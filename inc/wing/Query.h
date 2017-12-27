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
    Query(const Query&) = delete;                   ///< No copying
    Query(Query&& from);                  ///< Can move
    auto operator = (const Query&) = delete;        ///< No copy assign
    auto operator = (Query&&) -> Query&;  ///< Can move assign

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
