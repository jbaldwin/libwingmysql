#include "wing/Query.h"
#include "wing/EventLoop.h"
#include "wing/QueryPool.h"

namespace wing
{

Query::Query(QueryHandle* query_handle)
    : m_query_handle(query_handle)
{

}

Query::~Query()
{
    if(m_query_handle)
    {
        QueryPool& query_pool = m_query_handle->m_query_pool;
        query_pool.returnQuery(m_query_handle);
        m_query_handle = nullptr;
    }
}

Query::Query(Query&& from)
    : m_query_handle(from.m_query_handle)
{
    from.m_query_handle = nullptr;
}

auto Query::operator = (Query&& from) -> Query&
{
    m_query_handle = from.m_query_handle;
    from.m_query_handle = nullptr;
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
    return m_query_handle;
}

auto Query::operator ->() const -> const QueryHandle*
{
    return m_query_handle;
}

} // wing
