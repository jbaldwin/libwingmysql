#include "wing/QueryHandle.h"
#include "wing/EventLoop.h"
#include "wing/QueryPool.h"

namespace wing
{

QueryHandle::QueryHandle(
    std::unique_ptr<Query> query_handle
)
    :   m_query_handle(std::move(query_handle))
{

}

QueryHandle::~QueryHandle()
{
    if(m_query_handle)
    {
        QueryPool& query_pool = m_query_handle->m_query_pool;
        query_pool.returnQuery(std::move(m_query_handle));
    }
}

QueryHandle::QueryHandle(QueryHandle&& from)
    :   m_query_handle(std::move(from.m_query_handle))
{

}

auto QueryHandle::operator = (QueryHandle&& from) -> QueryHandle&
{
    m_query_handle = std::move(from.m_query_handle);
    return *this;
}

auto QueryHandle::operator *() -> Query&
{
    return *m_query_handle;
}

auto QueryHandle::operator *() const -> const Query&
{
    return *m_query_handle;
}

auto QueryHandle::operator -> () -> Query*
{
    return m_query_handle.get();
}

auto QueryHandle::operator ->() const -> const Query*
{
    return m_query_handle.get();
}

} // wing
