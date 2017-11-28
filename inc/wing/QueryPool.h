#pragma once

#include "wing/Query.h"
#include "wing/QueryHandle.h"
#include "wing/Connection.h"

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
    ~QueryPool() = default;

    explicit QueryPool(Connection connection);

    QueryPool(const QueryPool&) = delete;                       ///< No copying
    QueryPool(QueryPool&&) = default;                           ///< Can move
    auto operator = (const QueryPool&) -> QueryPool& = delete;  ///< No copy assign
    auto operator = (QueryPool&&) -> QueryPool& = default;      ///< Can move assign

    auto GetConnection() const -> const Connection&;

    auto Produce(
        const std::string& query,
        std::chrono::milliseconds timeout
    ) -> Query;
private:
    std::mutex m_lock;
    std::deque<std::unique_ptr<QueryHandle>> m_queries;
    Connection m_connection;
    EventLoop* m_event_loop;

    explicit QueryPool(
        Connection connection,
        EventLoop* event_loop
    );

    auto returnRequest(
        std::unique_ptr<QueryHandle> query_handle
    ) -> void;

    auto close() -> void;
};

} // wing
