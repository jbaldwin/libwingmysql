#pragma once

#include "wing/ConnectionInfo.hpp"
#include "wing/Query.hpp"
#include "wing/QueryHandle.hpp"

#include <chrono>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>

namespace wing {

class Executor;

class QueryPool {
    friend Executor;
    friend QueryHandle;

public:
    ~QueryPool();

    QueryPool(const QueryPool&) = delete;
    QueryPool(QueryPool&&) = delete;
    auto operator=(const QueryPool&) noexcept -> QueryPool& = delete;
    auto operator=(QueryPool&&) noexcept -> QueryPool& = delete;

    /**
     * @return The MySQL Server connection information for this pool.
     */
    auto Connection() const -> const ConnectionInfo& { return m_connection; }

    /**
     * Produces a Query from the pool with the provided query and timeout.
     * @param statement The SQL statement, can contain bind parameters.
     * @param timeout The timeout for this query in milliseconds.
     * @return A Query handle.
     */
    auto Produce(
        Statement statement,
        std::chrono::milliseconds timeout) -> QueryHandle;

    /**
     * Produces a Query from the pool with the provided query, timeout and on complete callback.
     * @param statement The SQL statement, can contain bind parameters.
     * @param timeout The timeout for this query in milliseconds.
     * @param on_complete Completion callback.
     * @return A Query handle.
     */
    auto Produce(
        Statement statement,
        std::chrono::milliseconds timeout,
        std::function<void(QueryHandle)> on_complete) -> QueryHandle;

    auto size() -> std::size_t
    {
        std::lock_guard<std::mutex> guard { m_lock };
        return m_queries.size();
    }

    auto clear() -> void
    {
        std::lock_guard<std::mutex> guard { m_lock };
        m_queries.clear();
    }

private:
    std::mutex m_lock;
    ConnectionInfo m_connection;
    std::deque<std::unique_ptr<Query>> m_queries;

    /**
     * Creates a query pool for running synchronous requests.
     * Queries created from this pool *CANNOT* be used in an EventLoop.
     *
     * @param connection The MySQL Server connection information.
     */
    explicit QueryPool(
        ConnectionInfo connection);

    auto returnQuery(
        std::unique_ptr<Query> query_handle_ptr) -> void;

    auto close() -> void;
};

} // wing
