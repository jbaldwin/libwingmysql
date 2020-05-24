#include "wing/QueryHandle.hpp"
#include "wing/QueryPool.hpp"

namespace wing {

QueryHandle::QueryHandle(
    std::unique_ptr<Query> query_ptr)
    : query_ptr(std::move(query_ptr))
{
}

QueryHandle::~QueryHandle()
{
    if (query_ptr != nullptr) {
        QueryPool& query_pool = query_ptr->m_query_pool;
        query_pool.returnQuery(std::move(query_ptr));
    }
}

QueryHandle::QueryHandle(QueryHandle&& from)
    : query_ptr(std::move(from.query_ptr))
{
}

auto QueryHandle::operator=(QueryHandle&& from) noexcept -> QueryHandle&
{
    query_ptr = std::move(from.query_ptr);
    return *this;
}

auto QueryHandle::operator*() -> Query&
{
    return *query_ptr;
}

auto QueryHandle::operator*() const -> const Query&
{
    return *query_ptr;
}

auto QueryHandle::operator-> () -> Query*
{
    return query_ptr.get();
}

auto QueryHandle::operator-> () const -> const Query*
{
    return query_ptr.get();
}

} // wing
