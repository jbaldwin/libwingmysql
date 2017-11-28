#pragma once

#include <string>

namespace wing
{

enum class QueryStatus
{
    BUILDING,               ///< The query is currently being built.
    EXECUTING,              ///< The query is currently executing.
    SUCCESS,                ///< The query completed succesfully.
    CONNECT_FAILURE,        ///< The query failed to connect to the server.
    WRITE_FAILURE,          ///< The query failed to write the query to the server.
    READ_FAILURE,           ///< The query failed to read the returned data.
    TIMEOUT,                ///< The query timed out.
    DISCONNECT,             ///< The MySQL server disconnected.
    SHUTDOWN_IN_PROGRESS    ///< The event loop is shutting down.
};

/**
 * Converts a query status into a string representation.
 * @param status The query status.
 * @return string
 */
auto query_status2str(
    QueryStatus status
) -> const std::string&;

} // wing
