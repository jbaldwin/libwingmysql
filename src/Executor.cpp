#include "wing/Executor.hpp"

#include <sys/syscall.h>
#include <unistd.h>

namespace wing {

Worker::Worker(
    std::thread thread)
    : m_thread(std::move(thread))
{
}

std::atomic<uint64_t> Executor::g_mysql_initialized { 0 };
std::mutex Executor::g_mysql_library_init_mutex {};

Executor::Executor(
    ConnectionInfo connection_info,
    std::size_t num_workers)
    : m_query_pool(std::move(connection_info))
{
    {
        // always grab lock so threads don't start before globally init'ed
        std::lock_guard<std::mutex> g { g_mysql_library_init_mutex };
        // If this thread is the first call init.
        if (g_mysql_initialized.fetch_add(1) == 0) {
            mysql_library_init(0, nullptr, nullptr);
        }
    }

    if (num_workers == 0) {
        num_workers = 1;
    }

    for (std::size_t i = 0; i < num_workers; ++i) {
        m_workers.emplace_back(
            Worker { std::thread {
                [this, i]() {
                    mysql_thread_init();
                    executor(i);
                    mysql_thread_end();
                } } });
    }
}

Executor::~Executor()
{
    Stop();
    // Join all workers before possibly shutting down the mysql library,
    // if there are multiple executors then mysql library won't be cleaned up yet.
    for (auto& worker : m_workers) {
        worker.m_thread.join();
    }

    {
        std::lock_guard<std::mutex> g { g_mysql_library_init_mutex };
        // If this is the last call end.
        if (g_mysql_initialized.fetch_sub(-1) == 1) {
            mysql_library_end();
        }
    }
}

auto Executor::StartQuery(
    wing::Statement statement,
    std::chrono::milliseconds timeout,
    std::function<void(QueryHandle)> on_complete) -> bool
{
    if (m_stop) {
        return false;
    }

    auto query_handle = m_query_pool.Produce(
        std::move(statement),
        timeout,
        std::move(on_complete));

    {
        std::lock_guard<std::mutex> g { m_query_queue_mutex };
        m_query_queue.emplace_back(std::move(query_handle));
    }

    ++m_active_query_count;

    m_wait_cv.notify_one();

    return true;
}

auto Executor::StartQuery(
    wing::Statement statement,
    std::chrono::milliseconds timeout) -> std::optional<std::future<QueryHandle>>
{
    std::optional<std::future<QueryHandle>> result {};

    // std::function is broken and requires everything to be copyable :(
    auto query_promise_ptr = std::make_shared<std::promise<QueryHandle>>();
    auto query_future = query_promise_ptr->get_future();

    if (StartQuery(
            std::move(statement),
            timeout,
            [p = std::move(query_promise_ptr)](QueryHandle query_handle) mutable {
                p->set_value(std::move(query_handle));
            })) {
        result.emplace(std::move(query_future));
    }

    return result;
}

auto Executor::executor(
    std::size_t worker_index) -> void
{
    using namespace std::chrono_literals;

    {
        auto& worker = m_workers[worker_index];
        worker.m_tid = syscall(SYS_gettid);
        // std::thread.native_handle() seems to be buggy?  Revert to using pthread_self().
        worker.m_native_handle = pthread_self();
    }

    while (!m_stop) {
        // Wait until there are queries ready to execute or this execution context is being stopped.
        {
            std::unique_lock<std::mutex> wait_lock { m_query_queue_mutex };
            m_wait_cv.wait(
                wait_lock,
                [this]() { return !m_query_queue.empty() || m_stop; });
        }

        while (true) {
            QueryHandle query_handle { nullptr };
            {
                std::lock_guard<std::mutex> g { m_query_queue_mutex };
                if (!m_query_queue.empty()) {
                    query_handle = std::move(m_query_queue.front());
                    m_query_queue.pop_front();
                }
            }

            if (query_handle.query_ptr != nullptr) {
                query_handle->execute();
                auto on_complete = std::move(query_handle->m_on_complete);
                on_complete(std::move(query_handle));
                --m_active_query_count;
            } else {
                // Ran out of queries to execute, go back to sleep or exit if m_stop.
                break;
            }
        }
    }
}

} // namespace wing
