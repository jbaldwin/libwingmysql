#include "wing/QueryStatus.h"

namespace wing
{

const std::string QUERY_STATUS_UNKNOWN              = "UNKNOWN";
const std::string QUERY_STATUS_BUILDING             = "BUILDING";
const std::string QUERY_STATUS_EXECUTING            = "EXECUTING";
const std::string QUERY_STATUS_CONNECT_FAILURE      = "CONNECT_FAILURE";
const std::string QUERY_STATUS_SUCCESS              = "SUCCESS";
const std::string QUERY_STATUS_WRITE_FAILURE        = "WRITE_FAILURE";
const std::string QUERY_STATUS_READ_FAILURE         = "READ_FAILURE";
const std::string QUERY_STATUS_STORE_FAILURE        = "STORE_FAILURE";
const std::string QUERY_STATUS_TIMEOUT              = "TIMEOUT";
const std::string QUERY_STATUS_DISCONNECT           = "DISCONNECT";
const std::string QUERY_STATUS_SHUTDOWN_IN_PROGRESS = "SHUTDOWN_IN_PROGRESS";

auto to_string(
    QueryStatus status
) -> const std::string&
{
    switch(status)
    {
        case QueryStatus::BUILDING:
            return QUERY_STATUS_BUILDING;
        case QueryStatus::EXECUTING:
            return QUERY_STATUS_EXECUTING;
        case QueryStatus::SUCCESS:
            return QUERY_STATUS_SUCCESS;
        case QueryStatus::CONNECT_FAILURE:
            return QUERY_STATUS_CONNECT_FAILURE;
        case QueryStatus::WRITE_FAILURE:
            return QUERY_STATUS_WRITE_FAILURE;
        case QueryStatus::READ_FAILURE:
            return QUERY_STATUS_READ_FAILURE;
        case QueryStatus::STORE_FAILURE:
            return QUERY_STATUS_STORE_FAILURE;
        case QueryStatus::TIMEOUT:
            return QUERY_STATUS_TIMEOUT;
        case QueryStatus::DISCONNECT:
            return QUERY_STATUS_DISCONNECT;
        case QueryStatus::SHUTDOWN_IN_PROGRESS:
            return QUERY_STATUS_SHUTDOWN_IN_PROGRESS;
        default:
            return QUERY_STATUS_UNKNOWN;
    }
}

} // wing
