#pragma once

#include <string>

namespace wing
{

enum class RequestStatus
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

auto request_status2str(
    RequestStatus status
) -> const std::string&;

} // wing
