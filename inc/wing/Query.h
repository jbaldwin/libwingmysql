#pragma once

#include "wing/QueryHandle.h"

#include <memory>

namespace wing
{

/**
 * This proxy object is used to modify the SQL query and
 * guarantee that the underlying object will always be returned
 * to the QueryPool for re-use.
 */
class Query
{
    friend class QueryPool;
    friend class EventLoop;
public:
    ~Query();
    Query(const Query&) = delete;
    Query(Query&& from);
    auto operator = (const Query&) = delete;
    auto operator = (Query&&) -> Query&;

    /**
     * Access to the SQL Query object.
     * @return Underlying query handle.
     * {@
     */
    auto operator * () -> QueryHandle&;
    auto operator * () const -> const QueryHandle&;
    auto operator -> () -> QueryHandle*;
    auto operator -> () const -> const QueryHandle*;
    /** @} */
private:
    explicit Query(
        std::unique_ptr<QueryHandle> query_handle
    );

    std::unique_ptr<QueryHandle> m_query_handle;
};

} // wing
