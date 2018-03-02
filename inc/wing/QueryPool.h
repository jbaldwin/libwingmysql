#pragma once

#include "wing/Query.h"
#include "wing/QueryHandle.h"
#include "wing/ConnectionInfo.h"

#include <deque>
#include <mutex>
#include <memory>
#include <chrono>

namespace wing
{

class EventLoop;

class QueryPool
{
    friend EventLoop;
    friend Query;
public:
    ~QueryPool();

    /**
     * Creates a query pool for running synchronous requests.
     * Queries created from this pool *CANNOT* be used in an EventLoop.
     *
     * @param connection The MySQL Server connection information.
     */
    explicit QueryPool(ConnectionInfo connection);

    QueryPool(const QueryPool&) = delete;                       ///< No copying
    QueryPool(QueryPool&&) = default;                           ///< Can move
    auto operator = (const QueryPool&) -> QueryPool& = delete;  ///< No copy assign
    auto operator = (QueryPool&&) -> QueryPool& = default;      ///< Can move assign

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
    ) -> Query;

    auto Produce(
        Statement statement,
        std::chrono::milliseconds timeout,
        OnCompleteHandler on_complete
    ) -> Query;
private:
    std::mutex m_lock;
    ConnectionInfo m_connection;
    EventLoop* m_event_loop;
    std::deque<std::unique_ptr<QueryHandle>> m_queries;

    explicit QueryPool(
        ConnectionInfo connection,
        EventLoop* event_loop
    );

    auto returnQuery(
        std::unique_ptr<QueryHandle> query_handle
    ) -> void;

    auto close() -> void;
};

} // wing
