#pragma once

#include <string>

namespace wing
{

enum class QueryStatus
{
    BUILDING,
    EXECUTING,
    SUCCESS,
    CONNECT_FAILURE,
    WRITE_FAILURE,
    READ_FAILURE,
    TIMEOUT,
    DISCONNECT,
    SHUTDOWN_IN_PROGRESS
};

auto query_status2str(
    QueryStatus status
) -> const std::string&;

} // wing
