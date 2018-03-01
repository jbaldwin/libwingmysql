#include "wing/Query.h"
#include "wing/EventLoop.h"
#include "wing/QueryPool.h"

namespace wing
{

Query::Query(
    std::unique_ptr<QueryHandle> query_handle
)
    : m_query_handle(std::move(query_handle))
{

}

Query::~Query()
{
    if(m_query_handle)
    {
        m_query_handle->Reset();
        QueryPool& query_pool = m_query_handle->m_query_pool;
        query_pool.returnQuery(std::move(m_query_handle));
    }
}

Query::Query(Query&& from)
    : m_query_handle(std::move(from.m_query_handle))
{

}

auto Query::operator = (Query&& from) -> Query&
{
    m_query_handle = std::move(from.m_query_handle);
    return *this;
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
