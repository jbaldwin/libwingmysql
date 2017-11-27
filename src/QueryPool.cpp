#include <wing/EventLoop.h>
#include "wing/QueryPool.h"

namespace wing
{

QueryPool::QueryPool(
    EventLoop* event_loop
)
    : m_lock(),
      m_requests(),
      m_event_loop(event_loop)
{

}

QueryPool::~QueryPool()
{

}

auto QueryPool::Produce(
    const std::string& query,
    std::chrono::milliseconds timeout
) -> Query
{
    m_lock.lock();
    if(m_requests.empty())
    {
        m_lock.unlock();
        RequestHandlePtr request_handle_ptr(
            new QueryHandle(
                m_event_loop,
                timeout,
                query
            )
        );

        return Query(std::move(request_handle_ptr));
    }
    else
    {
        auto request_handle_ptr = std::move(m_requests.back());
        m_requests.pop_back();
        m_lock.unlock();

        (*request_handle_ptr).SetTimeout(timeout);
        (*request_handle_ptr).SetQuery(query);

        return Query(std::move(request_handle_ptr));
    }
}

auto QueryPool::returnRequest(std::unique_ptr<QueryHandle> request_handle) -> void
{
    // If the handle has had any kind of error while processing
    // simply release the memory cand close it.
    // libuv will shutdown the poll/timer handles
    // and then delete the request handle.
    if((*request_handle).HasError())
    {
        auto* raw = request_handle.release();
        raw->close();
        return;
    }

    {
        (*request_handle).freeResult();
        std::lock_guard<std::mutex> guard(m_lock);
        m_requests.emplace_back(std::move(request_handle));
    }
}

auto QueryPool::close() -> void
{
    for(auto& request_handle : m_requests)
    {
        auto* raw = request_handle.release();
        raw->close();
    }
}

} // wing
