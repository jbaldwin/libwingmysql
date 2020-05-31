#pragma once

#include <string>

namespace wing {

enum class QueryStatus {
    /// The query is currently being built.
    BUILDING,
    /// Creating the query failed, possibly malformed or invalid.
    INVALID,
    /// The query is currently executing.
    EXECUTING,
    /// The query completed successfully.
    SUCCESS,
    /// The query failed to connect to the server.
    CONNECT_FAILURE,
    /// The query failed to write the query to the server.
    WRITE_FAILURE,
    /// The query failed to read the returned data.
    READ_FAILURE,
    /// The query failed to store the returned data.
    STORE_FAILURE,
    /// The query timed out.
    TIMEOUT,
    /// The MySQL server disconnected.
    DISCONNECT,
    /// Generic error, check the error string for more information.
    ERROR
};

/**
 * Converts a query status into a string representation.
 * @param status The query status.
 * @return string
 */
auto to_string(
    QueryStatus status) -> const std::string&;

} // wing
