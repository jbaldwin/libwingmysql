#include "wing/Request.h"
#include "wing/EventLoop.h"
#include "wing/RequestPool.h"

namespace wing
{

Request::Request(std::unique_ptr<RequestHandle> request_handle)
    : m_request_handle(std::move(request_handle))
{

}

Request::~Request() {
    if(m_request_handle) {
        RequestPool* request_pool = &(*m_request_handle).m_event_loop->m_request_pool;
        request_pool->returnRequest(std::move(m_request_handle));

    }
}

auto Request::operator *() -> RequestHandle&
{
    return *m_request_handle;
}

auto Request::operator *() const -> const RequestHandle&
{
    return *m_request_handle;
}

auto Request::operator -> () -> RequestHandle*
{
    return m_request_handle.get();
}

auto Request::operator ->() const -> const RequestHandle*
{
    return m_request_handle.get();
}

} // wing
