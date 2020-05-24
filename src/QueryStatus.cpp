#include "wing/QueryStatus.hpp"

namespace wing {

using namespace std::string_literals;

const std::string QUERY_STATUS_UNKNOWN = "UNKNOWN"s;
const std::string QUERY_STATUS_INVALID = "INVALID"s;
const std::string QUERY_STATUS_BUILDING = "BUILDING"s;
const std::string QUERY_STATUS_EXECUTING = "EXECUTING"s;
const std::string QUERY_STATUS_CONNECT_FAILURE = "CONNECT_FAILURE"s;
const std::string QUERY_STATUS_SUCCESS = "SUCCESS"s;
const std::string QUERY_STATUS_WRITE_FAILURE = "WRITE_FAILURE"s;
const std::string QUERY_STATUS_READ_FAILURE = "READ_FAILURE"s;
const std::string QUERY_STATUS_STORE_FAILURE = "STORE_FAILURE"s;
const std::string QUERY_STATUS_TIMEOUT = "TIMEOUT"s;
const std::string QUERY_STATUS_DISCONNECT = "DISCONNECT"s;

auto to_string(
    QueryStatus status) -> const std::string&
{
    switch (status) {
    case QueryStatus::BUILDING:
        return QUERY_STATUS_BUILDING;
    case QueryStatus::INVALID:
        return QUERY_STATUS_INVALID;
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
    default:
        return QUERY_STATUS_UNKNOWN;
    }
}

} // wing
