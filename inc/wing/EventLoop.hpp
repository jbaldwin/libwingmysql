#pragma once

#include "wing/ConnectionInfo.hpp"
#include "wing/QueryPool.hpp"

#include <atomic>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include <uv.h>

namespace wing {

class EventLoop {
    friend Query; ///< Required for libuv callbacks.
public:
    /**
     * Creates an event loop to execute asynchronous MySQL queries.
     * @param connection The connection information for the MySQL Server.
     */
    explicit EventLoop(
        ConnectionInfo connection);

    ~EventLoop();

    EventLoop(const EventLoop& copy) = delete;
    EventLoop(EventLoop&& move) = delete;
    auto operator=(const EventLoop& copy_assign) noexcept -> EventLoop& = delete;
    auto operator=(EventLoop&& move_assign) noexcept -> EventLoop& = delete;

    /**
     * @return True if the query and connect threads are running.
     */
    [[nodiscard]] auto IsRunning() -> bool;

    /**
     * This count does not include the number of queries that
     * require a new connection to the MySQL server and are
     * queued to connect.
     * @return Gets an active query count that is being executed.
     */
    [[nodiscard]] auto ActiveQueryCount() const -> uint64_t { return m_active_query_count; }

    /**
     * Stops the event loop and shuts down all current queries.
     * Queries not finished will be terminated via the IQueryCallback
     * with a QueryStatus::SHUTDOWN_IN_PROGRESS.
     */
    auto Stop() -> void;

    /**
     * Produces a query to set the details on to then be executed.
     * @param statement The statement to execute.
     * @param timeout The timeout for this query.
     * @param on_complete The on complete callback handler.
     * @return A query handle to set its query details on.
     */
    [[nodiscard]] auto ProduceQuery(
        wing::Statement statement,
        std::chrono::milliseconds timeout,
        std::function<void(QueryHandle)> on_complete) -> QueryHandle;

    /**
     * Starts an asynchronous query.
     * @param query The query to start.
     * @return True if the query started.
     */
    auto StartQuery(
        QueryHandle query) -> bool;

    /**
     * Gets the background event loop's native thread id.  This will only be available after the
     * background thread has started, e.g. `IsRunning()` returns `true`.
     * @return std::thread::native_handle_type for the background event loop thread.
     */
    [[nodiscard]] auto NativeThreadHandle() const -> const std::optional<std::atomic<std::thread::native_handle_type>>& { return m_native_handle; }

    /**
     * Gets the background event loop's native operating system thread id.  This will only be available
     * after the background thread has started, e.g. `IsRunning()` returns `true`.
     * @return gettid()
     */
    [[nodiscard]] auto OperatingSystemThreadId() const -> const std::optional<std::atomic<pid_t>>& { return m_tid; }

private:
    QueryPool m_query_pool;

    std::atomic<bool> m_is_query_running{ false };
    std::atomic<bool> m_is_stopping{ false };
    std::atomic<uint64_t> m_active_query_count{ 0 };

    std::thread m_background_query_thread;
    /// The background thread operating system thread id.
    std::optional<std::atomic<pid_t>> m_tid{};
    /// The background thread native handle type id (pthread_t).
    std::optional<std::atomic<std::thread::native_handle_type>> m_native_handle{};

    uv_loop_t* m_query_loop{ uv_loop_new() };
    uv_async_t m_query_async;
    std::atomic<bool> m_query_async_closed{ false };
    std::mutex m_pending_queries_lock;
    std::vector<QueryHandle> m_pending_queries;
    std::vector<QueryHandle> m_grabbed_queries;

    auto run_queries() -> void;

    auto callOnComplete(
        QueryHandle query) -> void;
    auto callOnComplete(
        std::unique_ptr<Query> query_handle) -> void;

    auto onClose(
        uv_handle_t* handle) -> void;

    auto requestsAcceptForQueryAsync(
        uv_async_t* async) -> void;

    friend auto uv_close_event_loop_callback(
        uv_handle_t* handle) -> void;

    friend auto on_complete_uv_query_execute_callback(
        uv_work_t* req,
        int status) -> void;

    friend auto requests_accept_for_query_async(
        uv_async_t* async) -> void;
};

} // wing
