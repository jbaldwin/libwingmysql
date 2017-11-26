#include "wing/RequestStatus.h"

namespace wing
{

const std::string REQUEST_STATUS_UNKNOWN        = "UNKNOWN";
const std::string REQUEST_STATUS_BUILDING       = "BUILDING";
const std::string REQUEST_STATUS_EXECUTING      = "EXECUTING";
const std::string REQUEST_STATUS_SUCCESS        = "SUCCESS";
const std::string REQUEST_STATUS_WRITE_FAILURE  = "WRITE_FAILURE";
const std::string REQUEST_STATUS_READ_FAILURE   = "READ_FAILURE";
const std::string REQUEST_STATUS_TIMEOUT        = "TIMEOUT";
const std::string REQUEST_STATUS_DISCONNECT     = "DISCONNECT";

auto request_status2str(
    RequestStatus status
) -> const std::string&
{
    switch(status)
    {
        case RequestStatus::BUILDING:
            return REQUEST_STATUS_BUILDING;
        case RequestStatus::EXECUTING:
            return REQUEST_STATUS_EXECUTING;
        case RequestStatus::SUCCESS:
            return REQUEST_STATUS_SUCCESS;
        case RequestStatus::WRITE_FAILURE:
            return REQUEST_STATUS_WRITE_FAILURE;
        case RequestStatus::READ_FAILURE:
            return REQUEST_STATUS_READ_FAILURE;
        case RequestStatus::TIMEOUT:
            return REQUEST_STATUS_TIMEOUT;
        case RequestStatus::DISCONNECT:
            return REQUEST_STATUS_DISCONNECT;
        default:
            return REQUEST_STATUS_UNKNOWN;
    }
}

} // wing
