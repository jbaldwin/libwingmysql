#pragma once

#include "wing/QueryPool.hpp"

#include <atomic>
#include <condition_variable>
#include <deque>
#include <future>
#include <mutex>
#include <thread>
#include <vector>

namespace wing {

class Executor;

class Worker {
    friend Executor;

public:
    Worker(const Worker&) = delete;
    Worker(Worker&&) = default;
    auto operator=(const Worker&) -> Worker& = delete;
    auto operator=(Worker &&) -> Worker& = default;

    ~Worker() = default;

    /**
     * Gets the worker's native thread id.  This will only be available after the worker thread
     * has started up.
     * @return std::thread::native_handle_type for the worker thread.
     */
    [[nodiscard]] auto NativeThreadHandle() const -> const std::optional<pid_t>& { return m_tid; }

    /**
     * Gets the worker's operating system thread id.  This will only be available after the worker
     * thread has started up.
     * @return gettid()
     */
    [[nodiscard]] auto OperatingSystemThreadId() const -> const std::optional<std::thread::native_handle_type>& { return m_native_handle; }

private:
    Worker(
        std::thread thread);

    std::thread m_thread;
    std::optional<pid_t> m_tid;
    std::optional<std::thread::native_handle_type> m_native_handle;
};

class Executor {
    friend Worker;

public:
    /**
     * @param connection_info The connection information for the MySQL Server.
     */
    explicit Executor(
        ConnectionInfo connection_info,
        std::size_t num_workers = 1);

    ~Executor();

    Executor(const Executor&) = delete;
    Executor(Executor&&) = delete;
    auto operator=(const Executor&) -> Executor& = delete;
    auto operator=(Executor &&) -> Executor& = delete;

    /**
     * @return The number of queries that are activitely executing and waiting to be executed.
     */
    [[nodiscard]] auto ActiveQueryCount() const -> uint64_t { return m_active_query_count; }

    /**
     * Stops the `Executor` workers and prevents futher queries submitted via
     * `StartQuery` from being executed, e.g. all futher calls to `StartQuery` after `Stop`
     * is called will always return `false`.
     * Note that any queries currently executing will be allow to finish or timeout and any queries
     * currently in the queue to be executed will also be allowed to finish or timeout.
     * An `Executor` cannot be re-started after stopping, a new fresh one must be created.
     */
    auto Stop() -> void
    {
        m_stop = true;
        m_wait_cv.notify_all();
    }

    /**
     * Starts a query with the given statement, timeout and on complete callback.  The on complete
     * callback will be call on the worker when the query is completed or timed out.
     * @param statement The statement to execute.
     * @param timeout The timeout for this query.
     * @param on_complete The on complete callback handler, this is called on the worker that
     *                    executed the query.
     * @return True if the query has been started or queued for execution.
     */
    [[nodiscard]] auto StartQuery(
        wing::Statement statement,
        std::chrono::milliseconds timeout,
        std::function<void(QueryHandle)> on_complete) -> bool;

    /**
     * Starts a query with the given statement and timeout.  If the query is started or queued
     * to be executed then a future to the query handle is given for when the query's execution
     * is completed.  The future can be blocked on until the query is completed.
     * @param statement The statement to execute.
     * @param timeout The timeout for this query.
     * @return If the query is queued for execution then a future is returned to block on until
     *         the query is completed or times out.
     */
    [[nodiscard]] auto StartQuery(
        wing::Statement statement,
        std::chrono::milliseconds timeout) -> std::optional<std::future<QueryHandle>>;

    /**
     * Gets the execution context's worker pool threads.  This can be useful for renaming
     * the thread's names etc.
     */
    auto Workers() const -> const std::vector<Worker>& { return m_workers; }

private:
    static std::atomic<uint64_t> g_mysql_initialized;
    static std::mutex g_mysql_library_init_mutex;

    QueryPool m_query_pool;

    std::atomic<bool> m_stop { false };
    std::atomic<uint64_t> m_active_query_count { 0 };

    std::vector<Worker> m_workers {};

    std::condition_variable m_wait_cv {};
    std::mutex m_query_queue_mutex {};
    std::deque<QueryHandle> m_query_queue {};

    auto executor(
        std::size_t worker_index) -> void;
};

} // namespace wing
