#pragma once

#include "wing/ConnectionInfo.hpp"
#include "wing/QueryStatus.hpp"
#include "wing/Row.hpp"
#include "wing/Statement.hpp"

#include <chrono>
#include <functional>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include <mysql/mysql.h>

namespace wing {

class QueryPool;
class QueryHandle;
class Executor;

class Query {
    friend QueryPool;
    friend QueryHandle;
    friend Executor;

public:
    ~Query();

    Query(const Query&) = delete;
    Query(Query&&) = delete;
    auto operator=(const Query&) noexcept -> Query& = delete;
    auto operator=(Query&&) noexcept -> Query& = delete;

    /**
     * @return Gets the query status after a completed query.
     */
    auto QueryStatus() const -> QueryStatus;

    /**
     * @return the original query that was executed
     */
    auto QueryOriginal() const -> const std::string&;

    /**
     * @return If no error then empty optional, otherwise the MySQL client error message.
     */
    auto Error() const -> std::optional<std::string>;

    /**
     * @return The number of fields returned from the query.
     */
    auto FieldCount() const -> size_t;

    /**
     * @return The number of rows returned from the query.
     */
    auto RowCount() const -> size_t;

    /**
     * @param idx The row to fetch.
     * @return The row.
     */
    auto Row(
        size_t idx) const -> const Row&;

    /**
     * @return The query result's rows.
     */
    auto Rows() const -> const std::vector<wing::Row>& { return m_rows; }

private:
    Query(
        QueryPool& query_pool,
        const ConnectionInfo& connection,
        std::function<void(QueryHandle)> on_complete,
        std::chrono::milliseconds timeout,
        wing::Statement statement);

    /**
     * Resets QueryHandle, freeing results and clearing m_query_parts and m_bind_params
     */
    auto reset() -> void;

    auto timeout() -> void;

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
    auto execute() -> wing::QueryStatus;

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

    /// Every query must have a query pool.
    QueryPool& m_query_pool;
    /// Connection information for the MySQL server.
    const ConnectionInfo& m_connection;
    /// On complete function handler for this query.
    std::function<void(QueryHandle)> m_on_complete;

    /// The timeout in milliseconds.
    std::chrono::milliseconds m_timeout;
    mutable MYSQL m_mysql;
    MYSQL_RES* m_result { nullptr };
    /// True if the result has already been parsed (to avoid doing it multiple times).
    bool m_parsed_result { false };
    /// The number of fields returned from the query.
    size_t m_field_count { 0 };
    /// The number of rows returned from the query.
    size_t m_row_count { 0 };
    /// User facing rows view.
    std::vector<wing::Row> m_rows;
    /// Has this MySQL client connected to the server yet?
    bool m_is_connected { false };
    /// Has this MySQL client had an error?
    bool m_had_error { false };

    /// The status of the last query.
    wing::QueryStatus m_query_status { QueryStatus::BUILDING };
    /// The SQL statement for this query
    wing::Statement m_statement;
    /// The SQL statement that was last executed
    std::string m_final_statement;
};

} // wing
