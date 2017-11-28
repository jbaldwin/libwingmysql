#pragma once

#include "wing/Query.h"

namespace wing
{

/**
 * The user defined query callback function for finished SQL queries.
 *
 * Use Query->GetStatus() -> QueryStatus to determine how the query finished.
 */
class IQueryCallback
{
public:
    virtual ~IQueryCallback() = default;
    /**
     * When a query finishes, successfully, timeout, read/write errors, etc this function
     * is called to notify the user.  This function is _ALWAYS_ called from the query event loop
     * thread -- even if it failed during connect.  This is done to reduce the complexity of
     * synchronizing in this callback as it will only ever be called from 1 thread in the library.
     * @param query The completed SQL query.
     */
    virtual auto OnComplete(
        wing::Query query
    ) -> void = 0;
};

} // wing