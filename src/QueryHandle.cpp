#include "wing/QueryHandle.hpp"
#include "wing/EventLoop.hpp"
#include "wing/QueryPool.hpp"

namespace wing {

QueryHandle::QueryHandle(
    std::unique_ptr<Query> query_handle_ptr)
    : m_query_handle_ptr(std::move(query_handle_ptr))
{
}

QueryHandle::~QueryHandle()
{
    if (m_query_handle_ptr) {
        QueryPool& query_pool = m_query_handle_ptr->m_query_pool;
        query_pool.returnQuery(std::move(m_query_handle_ptr));
    }
}

QueryHandle::QueryHandle(QueryHandle&& from)
    : m_query_handle_ptr(std::move(from.m_query_handle_ptr))
{
}

auto QueryHandle::operator=(QueryHandle&& from) noexcept -> QueryHandle&
{
    m_query_handle_ptr = std::move(from.m_query_handle_ptr);
    return *this;
}

auto QueryHandle::operator*() -> Query&
{
    return *m_query_handle_ptr;
}

auto QueryHandle::operator*() const -> const Query&
{
    return *m_query_handle_ptr;
}

auto QueryHandle::operator-> () -> Query*
{
    return m_query_handle_ptr.get();
}

auto QueryHandle::operator-> () const -> const Query*
{
    return m_query_handle_ptr.get();
}

} // wing
