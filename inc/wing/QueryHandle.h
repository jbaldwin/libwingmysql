#pragma once

#include "wing/QueryStatus.h"
#include "wing/Row.h"
#include "wing/ConnectionInfo.h"
#include "wing/Statement.h"

#include <string>
#include <memory>
#include <chrono>
#include <sstream>
#include <functional>

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
     * Resets QueryHandle, freeing results and clearing m_query_parts and m_bind_params
     */
    auto Reset() -> void;

    /**
     * @param on_complete The on complete function handle for this query.
     */
    auto SetOnCompleteHandler(
        std::function<void(Query)> on_complete
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
     * Sets the query statement for this query, the statement object
     * provides utilities for creating prepared statements and binding
     * the parameters
     * @param statement the statement set for this query
     */
    auto SetStatement(
        Statement statement
    ) -> void;

    /**
     * @return the original query that was executed
     */
    auto GetQueryOriginal() const -> const std::string&;

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
    auto GetRow(
        size_t idx
    ) const -> const Row&;

    /**
     * @param user_data Sets a user defined data pointer for this query.
     * @deprecated Use std::bind or lambda captures via OnCompleteHandler.
     */
    [[deprecated]]
    auto SetUserData(
        void* user_data
    ) -> void;

    /**
     * @return Gets the user defined data pointer for this query.
     * @deprecated Use std::bind or lambda captures via OnCompleteHandler.
     */
    [[deprecated]]
    auto GetUserData() const -> void*;

private:
    QueryHandle(
        EventLoop* event_loop,
        QueryPool& query_pool,
        const ConnectionInfo& connection,
        std::function<void(Query)> on_complete,
        std::chrono::milliseconds timeout,
        Statement statement
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
     * Parses the rows from the result.
     */
    auto parseRows() -> void;

    /**
     * Frees the previous query result.
     */
    auto freeResult() -> void;

    EventLoop* m_event_loop;                  ///< Asynchronous queries will have an event loop. nullptr for synchronous requests.
    QueryPool& m_query_pool;                  ///< Every query must have a query pool.
    const ConnectionInfo& m_connection;       ///< Connection information for the MySQL server.
    std::function<void(Query)> m_on_complete; ///< On complete function handler for this query.

    std::chrono::milliseconds m_timeout;///< The timeout in milliseconds.
    MYSQL m_mysql;
    MYSQL_RES* m_result;
    bool m_parsed_result;               ///< True if the result has already been parsed (to avoid doing it multiple times).
    size_t m_field_count;               ///< The number of fields returned from the query.
    size_t m_row_count;                 ///< The number of rows returned from the query.
    std::vector<Row> m_rows;            ///< User facing rows view.
    bool m_is_connected;                ///< Has this MySQL client connected to the server yet?
    bool m_had_error;                   ///< Has this MySQL client had an error?

    QueryStatus m_query_status;         ///< The status of the last query.
    Statement m_statement;              ///< The SQL statement for this query
    std::string m_final_statement{};    ///< The SQL statement that was last executed

    void* m_user_data;                  ///< User provided data.

    friend auto on_uv_query_execute_callback(
        uv_work_t* req
    ) -> void;

    friend auto on_complete_uv_query_execute_callback(
        uv_work_t* req,
        int status
    ) -> void;
};

} // wing
