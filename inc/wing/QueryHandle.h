#pragma once

#include "wing/QueryStatus.h"
#include "wing/Row.h"
#include "wing/ConnectionInfo.h"

#include <string>
#include <memory>
#include <chrono>
#include <sstream>

#include <uv.h>
#include <mysql/mysql.h>

namespace wing
{

class EventLoop;
class QueryPool;
class Query;

using OnCompleteHandler = void(*)(Query);

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

    /**
     * @param on_complete The on complete function handle for this query.
     */
    auto SetOnCompleteHandler(
        OnCompleteHandler on_complete
    ) -> void;

    /**
     * @return Gets the query status after a completed query.
     */
    auto GetQueryStatus() const -> QueryStatus;

    /**
     * @param timeout The timeout for this query in milliseconds.
     */
    auto SetTimeout(
        std::chrono::milliseconds timeout
    ) -> void;

    /**
     * @return The timeout for this query in milliseconds.
     */
    auto GetTimeout() const -> std::chrono::milliseconds;

    /**
     * Sets the query string.  To use bind parameters set question marks '?'
     * in the places where you want to bind values to.
     *
     * Note that strings do not automatically include quotes when binding, you must
     * include them in the template query.
     *
     * Parameters are not bound by an index, they are bound in sequence, 0, 1, 2, etc.
     *
     * @param query SQL query to execute.  Can contain parameters to bind.
     */
    auto SetQuery(
        std::string query
    ) -> void;

    /**
     * @return Gets the original un-bind'ed SQL query.
     */
    auto GetQueryOriginal() const -> const std::string&;

    /**
     * Note that this is lazily evaluated and will not be valid
     * until after the query is completed.
     * @return Gets the bind'ed SQL query.
     */
    auto GetQueryWithBindParams() const -> const std::string&;

    /**
     * Escapes and binds a string parameter to the query in the next parameter slot.
     *
     * Note that this function does not include quotes around the string parameter,
     * you must include them in the template query when using SetQuery().
     *
     * example: SELECT * FROM FOO WHERE key_id = '?';
     *                                           ^-^-----
     *
     * @param param The string parameter to escape and bind.
     */
    auto BindString(
        const std::string& param
    ) -> void;

    /**
     * Binds an unsigned 64 bit integer parameter to the query in the next parameter slot.
     * @param param The unsigned 64 bit integer parameter to bind.
     */
    auto BindUInt64(
        uint64_t param
    ) -> void;

    /**
     * Binds a signed 64 bit integer parameter to the query in the next parameter slot.
     * @param param The signed 64 bit integer parameter to bind.
     */
    auto BindInt64(
        int64_t param
    ) -> void;

    /**
     * Executes the query synchronously.
     *
     * Note that synchronous/asynchronous queries cannot be mixed
     * on an EventLoop.  If you are using an EventLoop you *MUST*
     * use its QueryPool otherwise the Query will not know how to
     * execute properly.
     *
     * In this mode the query does not require an EventLoop object,
     * however it will block until it finishes or times out.
     * @return The status of the query, e.g. success or timeout.
     */
    auto Execute() -> QueryStatus;

    /**
     * @return True if the query had an error while executing.
     */
    auto HasError() const -> bool;

    /**
     * @return The MySQL client error message.
     */
    auto GetError() -> std::string;

    /**
     * @return The number of fields returned from the query.
     */
    auto GetFieldCount() const -> size_t;

    /**
     * @return The number of rows returned from the query.
     */
    auto GetRowCount() const -> size_t;

    /**
     * @param idx The row to fetch.
     * @return The row.
     */
    auto GetRow(size_t idx) const -> const Row&;

private:
    QueryHandle(
        EventLoop* event_loop,
        QueryPool& query_pool,
        const ConnectionInfo& connection,
        OnCompleteHandler on_complete,
        std::chrono::milliseconds timeout,
        std::string query
    );

    /**
     * If the handle has not yet connected, this will block and connect.
     *
     * This function should only be run on the connect background thread
     * for asynchronous queries.
     *
     * @return True on connected, false on failure.
     */
    auto connect() -> bool;

    /**
     * Initializes various fields for an asynchronous query.
     */
    auto startAsync() -> void;

    /**
     * Escapes (strings) and binds parameters to the template query.
     */
    auto bindParameters() -> void;

    /**
     * Called when an asynchronous query fails.
     * @param status The reason why the query failed.
     */
    auto failedAsync(QueryStatus status) -> void;

    /**
     * Called when the socket has data to read (query is finished).
     * @return True if the contents were read successfully.
     */
    auto onRead() -> bool;

    /**
     * Called when the socket can send a query to the MySQL server.
     * @return True if the query was written successfully.
     */
    auto onWrite() -> bool;

    /**
     * Called when the query has timed out.
     */
    auto onTimeout() -> void;

    /**
     * Called when the socket is disconnected by the MySQL server.
     */
    auto onDisconnect() -> void;

    /**
     * Parses the rows from the result.
     */
    auto parseRows() -> void;

    /**
     * Frees the previous query result.
     */
    auto freeResult() -> void;

    /**
     * For asynchronous queries the libuv internals must be cleaned up
     * before destructing the object.  This will tell libuv to cleanup
     * its resources, and then in its callback will delete this object.
     */
    auto close() -> void;

    EventLoop* m_event_loop;            ///< Asynchronous queries will have an event loop. nullptr for synchronous requests.
    QueryPool& m_query_pool;            ///< Every query must have a query pool.
    const ConnectionInfo& m_connection; ///< Connection information for the MySQL server.
    OnCompleteHandler m_on_complete;    ///< On complete function handler for this query.

    uv_poll_t m_poll;                   ///< Asynchronous queries libuv polls tructure for read/write notifications.
    uv_timer_t m_timeout_timer;         ///< libuv timeout for the query.
    std::chrono::milliseconds m_timeout;///< The timeout in milliseconds.
    MYSQL m_mysql;
    MYSQL_RES* m_result;
    bool m_parsed_result;               ///< True if the result has already been parsed (to avoid doing it multiple times).
    size_t m_field_count;               ///< The number of fields returned from the query.
    size_t m_row_count;                 ///< The number of rows returned from the query.
    std::vector<Row> m_rows;            ///< User facing rows view.
    bool m_is_connected;                ///< Has this MySQL client connected to the server yet?
    bool m_had_error;                   ///< Has this MySQL client had an error?

    QueryStatus m_query_status;             ///< The status of the last query.
    std::string m_original_query;           ///< The originally provided SQL query string - no bound parameters.
    std::string m_query_buffer;             ///< Buffer for binding parameters.
    std::vector<StringView> m_query_parts;  ///< The query parts before/after a parameter '?'.
    size_t m_bind_param_count;              ///< The number of parameters to bind to this query.
    std::vector<std::string> m_bind_params; ///< The escaped and stringified bind parameters.

    bool m_poll_closed;             ///< libuv flag to know when the poller has been closed.
    bool m_timeout_timer_closed;    ///< libuv flag to know when the timeout timer has been closed.

    std::stringstream m_converter;  ///< Convert non-string bind parameters into strings.

    friend auto on_uv_poll_callback(
        uv_poll_t* handle,
        int status,
        int events
    ) -> void; ///< for grabbing the event loop.

    friend auto on_uv_timeout_callback(
        uv_timer_t* handle
    ) -> void; ///< for grabbing the event loop.

    friend auto on_uv_close_query_handle_callback(
        uv_handle_t* handle
    ) -> void; ///< for closing/deleting this handle
};

} // wing
