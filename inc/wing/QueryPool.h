#pragma once

#include "wing/QueryHandle.h"
#include "wing/Query.h"
#include "wing/ConnectionInfo.h"

#include <deque>
#include <mutex>
#include <memory>
#include <chrono>
#include <functional>

namespace wing
{

class EventLoop;

class QueryPool
{
    friend EventLoop;
    friend QueryHandle;
public:
    ~QueryPool();

    /**
     * Creates a query pool for running synchronous requests.
     * Queries created from this pool *CANNOT* be used in an EventLoop.
     *
     * @param connection The MySQL Server connection information.
     */
    explicit QueryPool(ConnectionInfo connection);

    QueryPool(const QueryPool&) = delete;
    QueryPool(QueryPool&&) = delete;
    auto operator = (const QueryPool&) -> QueryPool& = delete;
    auto operator = (QueryPool&&) -> QueryPool& = delete;

    /**
     * @return The MySQL Server connection information for this pool.
     */
    auto GetConnection() const -> const ConnectionInfo&;

    /**
     * Produces a Query from the pool with the provided query and timeout.
     * @param statement The SQL statement, can contain bind parameters.
     * @param timeout The timeout for this query in milliseconds.
     * @return A Query handle.
     */
    auto Produce(
        Statement statement,
        std::chrono::milliseconds timeout
    ) -> QueryHandle;

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
        std::function<void(QueryHandle)> on_complete
    ) -> QueryHandle;
private:
    std::mutex m_lock;
    ConnectionInfo m_connection;
    EventLoop* m_event_loop{nullptr};
    std::deque<std::unique_ptr<Query>> m_queries;

    explicit QueryPool(
        ConnectionInfo connection,
        EventLoop* event_loop
    );

    auto returnQuery(
        std::unique_ptr<Query> query_handle
    ) -> void;

    auto close() -> void;
};

} // wing
