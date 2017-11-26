#pragma once

#include <string>

namespace wing
{

enum class RequestStatus
{
    BUILDING,
    EXECUTING,
    SUCCESS,
    WRITE_FAILURE,
    READ_FAILURE,
    TIMEOUT,
    DISCONNECT
};

auto request_status2str(
    RequestStatus status
) -> const std::string&;

} // wing
