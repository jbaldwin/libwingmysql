#pragma once

#include "wing/QueryStatus.h"
#include "wing/Row.h"

#include <uv.h>
#include <mysql/mysql.h>

#include <string>
#include <memory>
#include <chrono>

namespace wing
{

class EventLoop;
class QueryPool;
class Query;

class QueryHandle
{
    friend EventLoop;
    friend QueryPool;
    friend Query;

public:
    ~QueryHandle();

    QueryHandle(const QueryHandle&) = delete;                   ///< No copying
    QueryHandle(QueryHandle&&) = default;                       ///< Can move
    auto operator = (const QueryHandle&) = delete;              ///< No copy assign
    auto operator = (QueryHandle&&) -> QueryHandle& = default;  ///< Can move assign

    auto GetRequestStatus() const -> QueryStatus;

    auto SetTimeout(std::chrono::milliseconds timeout) -> void;
    auto GetTimeout() const -> std::chrono::milliseconds;
    auto SetQuery(const std::string& query) -> void;
    auto GetQuery() const -> const std::string&;

    auto HasError() const -> bool;
    auto GetError() -> std::string;

    auto GetFieldCount() const -> size_t;
    auto GetRowCount() const -> size_t;
    auto GetRows() const -> const std::vector<Row>&;

private:
    QueryHandle(
        EventLoop* event_loop,
        std::chrono::milliseconds timeout,
        std::string query
    );

    auto connect() -> bool;
    auto start() -> void;
    auto failed(QueryStatus status) -> void;

    auto onRead() -> bool;
    auto onWrite() -> bool;
    auto onTimeout() -> void;
    auto onDisconnect() -> void;

    auto freeResult() -> void;

    auto close() -> void;

    EventLoop* m_event_loop;

    uv_poll_t m_poll;
    uv_timer_t m_timeout_timer;
    std::chrono::milliseconds m_timeout;
    MYSQL m_mysql;
    MYSQL_RES* m_result;
    mutable bool m_parsed_result;
    size_t m_field_count;
    size_t m_row_count;
    mutable std::vector<Row> m_rows;
    bool m_is_connected;
    bool m_had_error;

    QueryStatus m_request_status;
    std::string m_query;

    bool m_poll_closed;
    bool m_timeout_timer_closed;

    friend auto on_uv_poll_callback(
        uv_poll_t* handle,
        int status,
        int events
    ) -> void; ///< for grabbing the event loop.

    friend auto on_uv_timeout_callback(
        uv_timer_t* handle
    ) -> void; ///< for grabbing the event loop.

    friend auto on_uv_close_request_handle_callback(
        uv_handle_t* handle
    ) -> void; ///< for closing/deleting this handle
};

using RequestHandlePtr = std::unique_ptr<QueryHandle>;

} // wing
