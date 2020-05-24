#pragma once

#include "wing/Query.hpp"

#include <memory>

namespace wing {

/**
 * This proxy object is used to modify the SQL query and
 * guarantee that the underlying object will always be returned
 * to the QueryPool for re-use.
 */
class QueryHandle {
    friend class QueryPool;
    friend class Executor;

public:
    ~QueryHandle();
    QueryHandle(const QueryHandle&) = delete;
    QueryHandle(QueryHandle&& from);
    auto operator=(const QueryHandle&) noexcept -> QueryHandle& = delete;
    auto operator=(QueryHandle&&) noexcept -> QueryHandle&;

    /**
     * Access to the SQL Query object.
     * @return Underlying query handle.
     * {@
     */
    auto operator*() -> Query&;
    auto operator*() const -> const Query&;
    auto operator-> () -> Query*;
    auto operator-> () const -> const Query*;
    /** @} */
private:
    explicit QueryHandle(
        std::unique_ptr<Query> query_ptr);

    std::unique_ptr<Query> query_ptr;
};

} // wing
