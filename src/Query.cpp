#include "wing/Query.hpp"
#include "wing/QueryPool.hpp"
#include "wing/Util.hpp"

#include <mysql/mysql.h>

#include <sstream>
#include <stdexcept>

namespace wing {

Query::~Query()
{
    reset();
    mysql_close(&m_mysql);
}

auto Query::QueryStatus() const -> wing::QueryStatus
{
    return m_query_status;
}

auto Query::QueryOriginal() const -> const std::string&
{
    return m_final_statement;
}

auto Query::Error() const -> std::optional<std::string>
{
    if (m_had_error) {
        return { mysql_error(&m_mysql) };
    } else {
        return {};
    }
}

auto Query::ErrorOr(
    std::string default_msg) -> std::string
{
    auto error = Error();
    return error.has_value() ? error.value() : std::move(default_msg);
}

auto Query::FieldCount() const -> size_t
{
    return m_field_count;
}

auto Query::RowCount() const -> size_t
{
    return m_row_count;
}

auto Query::Row(size_t idx) const -> const wing::Row&
{
    return m_rows.at(idx);
}

Query::Query(
    QueryPool& query_pool,
    const ConnectionInfo& connection,
    std::function<void(QueryHandle)> on_complete,
    std::chrono::milliseconds timeout,
    wing::Statement statement)
    : m_query_pool(query_pool)
    , m_connection(connection)
    , m_on_complete(std::move(on_complete))
    , m_timeout(timeout)
    , m_statement(std::move(statement))
{
    mysql_init(&m_mysql);

    this->timeout();

    unsigned int connect_timeout = 1;
    mysql_options(&m_mysql, MYSQL_OPT_CONNECT_TIMEOUT, &connect_timeout);

    uint64_t auto_reconnect = 1;
    mysql_options(&m_mysql, MYSQL_OPT_RECONNECT, &auto_reconnect);

#ifdef WING_MYSQL_OPT_SSL_ENFORCE_DISABLED
    bool ssl = false;
    mysql_options(&m_mysql, MYSQL_OPT_SSL_ENFORCE, &ssl);
#endif

    //  Some mysql libraries do not support SSL_MODE_DISABLED
#ifdef WING_PERCONA_SSL_DISABLED
    uint32_t ssl_mode = SSL_MODE_DISABLED;
    mysql_options(&m_mysql, MYSQL_OPT_SSL_MODE, &ssl_mode);
#endif
}

auto Query::reset() -> void
{
    freeResult();
    m_statement = wing::Statement {};
    m_query_status = QueryStatus::BUILDING;
    m_had_error = false;
}

auto Query::timeout() -> void
{
    auto timeout_seconds = std::chrono::duration_cast<std::chrono::seconds>(m_timeout);
    unsigned int read_timeout = static_cast<unsigned int>(timeout_seconds.count());
    mysql_options(&m_mysql, MYSQL_OPT_READ_TIMEOUT, &read_timeout);
}

auto Query::execute() -> wing::QueryStatus
{
    if (!m_is_connected) {
        if (!connect()) {
            m_query_status = QueryStatus::CONNECT_FAILURE;
            m_had_error = true;
            return m_query_status;
        }
    }

    freeResult();

    try {
        // ask the statement to prepare the final query string
        m_final_statement = m_statement.prepareStatement(
            [this](const std::string& str_value) {
                if (str_value.empty()) {
                    throw std::invalid_argument("Empty statement part passed in to prepareStatement");
                }
                // https://dev.mysql.com/doc/refman/5.7/en/mysql-real-escape-string.html
                std::string buffer;
                buffer.resize(str_value.length() * 2 + 1);

                size_t length = mysql_real_escape_string(&m_mysql, buffer.data(), &str_value.front(), str_value.length());
                buffer.resize(length);

                return buffer;
            });
    } catch (const std::invalid_argument& e) {
        m_query_status = QueryStatus::INVALID;
        m_had_error = true;
        return m_query_status;
    }

    if (0 == mysql_real_query(&m_mysql, m_final_statement.c_str(), m_final_statement.length())) {
        m_result = mysql_store_result(&m_mysql);
        if (m_result != nullptr) {
            m_query_status = QueryStatus::SUCCESS;
            m_field_count = mysql_num_fields(m_result);
            m_row_count = mysql_num_rows(m_result);
            parseRows();
        } else {
            // Use this function to determine if the query should have returned values
            if (mysql_field_count(&m_mysql) == 0) {
                m_query_status = QueryStatus::SUCCESS;
                m_field_count = 0;
                m_row_count = mysql_affected_rows(&m_mysql);
                m_parsed_result = true;
            } else {
                m_query_status = QueryStatus::ERROR;
                m_had_error = true;
            }
        }
    } else {
        m_query_status = QueryStatus::ERROR;
        m_had_error = true;
    }
    return m_query_status;
}

auto Query::connect() -> bool
{
    if (!m_is_connected) {
        auto* success = mysql_real_connect(
            &m_mysql,
            m_connection.Host().c_str(),
            m_connection.User().c_str(),
            m_connection.Password().c_str(),
            m_connection.Database().c_str(),
            m_connection.Port(),
            m_connection.Socket().c_str(),
            m_connection.ClientFlags());

        if (success == nullptr) {
            return false;
        }

        m_is_connected = true;
    }

    return true;
}

auto Query::parseRows() -> void
{
    if (!m_parsed_result && RowCount() > 0) {
        m_parsed_result = true;
        m_rows.reserve(RowCount());
        MYSQL_ROW mysql_row = nullptr;
        while ((mysql_row = mysql_fetch_row(m_result))) {
            auto lengths = mysql_fetch_lengths(m_result);
            wing::Row row(mysql_row, m_field_count, lengths);
            m_rows.emplace_back(std::move(row));
        }
    }
}

auto Query::freeResult() -> void
{
    if (m_result != nullptr) {
        mysql_free_result(m_result);
        m_parsed_result = false;
        m_field_count = 0;
        m_row_count = 0;
        m_rows.clear();
        m_result = nullptr;
    }
}

} // wing
