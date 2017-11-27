#pragma once

#include "wing/Query.h"
#include "wing/QueryHandle.h"

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

    QueryPool(const QueryPool&) = delete;                       ///< No copying
    QueryPool(QueryPool&&) = default;                           ///< Can move
    auto operator = (const QueryPool&) -> QueryPool& = delete;  ///< No copy assign
    auto operator = (QueryPool&&) -> QueryPool& = default;      ///< Can move assign

    auto Produce(
        const std::string& query,
        std::chrono::milliseconds timeout
    ) -> Query;
private:
    std::mutex m_lock;
    std::deque<std::unique_ptr<QueryHandle>> m_requests;
    EventLoop* m_event_loop;

    explicit QueryPool(
        EventLoop* event_loop
    );

    auto returnRequest(
        std::unique_ptr<QueryHandle> request_handle
    ) -> void;

    auto close() -> void;
};

} // wing
