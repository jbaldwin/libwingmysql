#include "wing/Query.h"
#include "wing/EventLoop.h"
#include "wing/QueryPool.h"

namespace wing
{

Query::Query(std::unique_ptr<QueryHandle> request_handle)
    : m_request_handle(std::move(request_handle))
{

}

Query::~Query() {
    if(m_request_handle) {
        QueryPool* request_pool = &(*m_request_handle).m_event_loop->m_request_pool;
        request_pool->returnRequest(std::move(m_request_handle));

    }
}

auto Query::operator *() -> QueryHandle&
{
    return *m_request_handle;
}

auto Query::operator *() const -> const QueryHandle&
{
    return *m_request_handle;
}

auto Query::operator -> () -> QueryHandle*
{
    return m_request_handle.get();
}

auto Query::operator ->() const -> const QueryHandle*
{
    return m_request_handle.get();
}

} // wing
