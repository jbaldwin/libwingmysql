#include "wing/Query.h"
#include "wing/EventLoop.h"
#include "wing/QueryPool.h"

namespace wing
{

Query::Query(std::unique_ptr<QueryHandle> query_handle)
    : m_query_handle(std::move(query_handle))
{

}

Query::~Query() {
    if(m_query_handle) {
        QueryPool* query_pool = (*m_query_handle).m_query_pool;
        query_pool->returnRequest(std::move(m_query_handle));

    }
}

auto Query::operator *() -> QueryHandle&
{
    return *m_query_handle;
}

auto Query::operator *() const -> const QueryHandle&
{
    return *m_query_handle;
}

auto Query::operator -> () -> QueryHandle*
{
    return m_query_handle.get();
}

auto Query::operator ->() const -> const QueryHandle*
{
    return m_query_handle.get();
}

} // wing
