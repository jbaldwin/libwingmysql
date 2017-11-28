#pragma once

#include "wing/QueryStatus.h"
#include "wing/Row.h"
#include "wing/Connection.h"

#include <uv.h>
#include <mysql/mysql.h>

#include <string>
#include <memory>
#include <chrono>
#include <sstream>

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
    auto SetQuery(std::string query) -> void;
    auto GetQueryOriginal() const -> const std::string&;
    auto GetQueryWithBindParams() const -> const std::string&;

    auto BindString(const std::string& param) -> void;
    auto BindUInt64(uint64_t param) -> void;
    auto BindInt64(int64_t param) -> void;

    auto Execute() -> QueryStatus;

    auto HasError() const -> bool;
    auto GetError() -> std::string;

    auto GetFieldCount() const -> size_t;
    auto GetRowCount() const -> size_t;
    auto GetRows() const -> const std::vector<Row>&;

private:
    QueryHandle(
        EventLoop* event_loop,
        QueryPool* query_pool,
        const Connection& connection,
        std::chrono::milliseconds timeout,
        std::string query
    );

    auto connect() -> bool;
    auto startAsync() -> void;
    auto bindParameters() -> void;
    auto failedAsync(QueryStatus status) -> void;

    auto onRead() -> bool;
    auto onWrite() -> bool;
    auto onTimeout() -> void;
    auto onDisconnect() -> void;

    auto freeResult() -> void;

    auto close() -> void;

    EventLoop* m_event_loop;
    QueryPool* m_query_pool;
    const Connection& m_connection;

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

    QueryStatus m_query_status;
    std::string m_original_query;
    std::string m_query_buffer;
    std::vector<StringView> m_query_parts;
    size_t m_bind_param_count;
    std::vector<std::string> m_bind_params;

    bool m_poll_closed;
    bool m_timeout_timer_closed;

    std::stringstream m_converter;

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
