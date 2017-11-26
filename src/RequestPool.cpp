#include <wing/EventLoop.h>
#include "wing/RequestPool.h"

namespace wing
{

RequestPool::RequestPool(
    EventLoop* event_loop
)
    : m_lock(),
      m_requests(),
      m_event_loop(event_loop)
{

}

RequestPool::~RequestPool()
{
    for(auto& request_handle : m_requests)
    {
        auto* raw = request_handle.release();
        raw->close();
    }
}

auto RequestPool::Produce(
    const std::string& query,
    std::chrono::milliseconds timeout
) -> Request
{
    m_lock.lock();
    if(m_requests.empty())
    {
        m_lock.unlock();
        RequestHandlePtr request_handle_ptr(
            new RequestHandle(
                m_event_loop,
                timeout,
                query,
                m_event_loop->m_host,
                m_event_loop->m_port,
                m_event_loop->m_user,
                m_event_loop->m_password,
                m_event_loop->m_db,
                m_event_loop->m_client_flags
            )
        );

        return Request(std::move(request_handle_ptr));
    }
    else
    {
        auto request_handle_ptr = std::move(m_requests.back());
        m_requests.pop_back();
        m_lock.unlock();

        (*request_handle_ptr).SetTimeout(timeout);
        (*request_handle_ptr).SetQuery(query);

        return Request(std::move(request_handle_ptr));
    }
}

auto RequestPool::returnRequest(std::unique_ptr<RequestHandle> request_handle) -> void
{
    // If the handle has had any kind of error while processing
    // simply release the memory cand close it.
    // libuv will shutdown the poll/timer handles
    // and then delete the request handle.
    if((*request_handle).m_is_connected)
    {
        auto* raw = request_handle.release();
        raw->close();
        return;
    }

    {
        std::lock_guard<std::mutex> guard(m_lock);
        m_requests.emplace_back(std::move(request_handle));
    }
}

} // wing
