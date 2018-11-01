#include "wing/QueryHandle.h"
#include "wing/EventLoop.h"
#include "wing/QueryPool.h"
#include "wing/Util.h"

#include <mysql/mysql.h>

#include <stdexcept>
#include <sstream>

namespace wing
{

QueryHandle::QueryHandle(
    EventLoop* event_loop,
    QueryPool& query_pool,
    const ConnectionInfo& connection,
    std::function<void(Query)> on_complete,
    std::chrono::milliseconds timeout,
    Statement statement
)
    : m_event_loop(event_loop),
      m_query_pool(query_pool),
      m_connection(connection),
      m_on_complete(std::move(on_complete)),
      m_timeout(timeout),
      m_mysql(),
      m_result(nullptr),
      m_parsed_result(false),
      m_field_count(0),
      m_row_count(0),
      m_rows(),
      m_is_connected(false),
      m_had_error(false),
      m_query_status(QueryStatus::BUILDING),
      m_statement(std::move(statement)),
      m_user_data(nullptr)
{
    mysql_init(&m_mysql);

    SetTimeout(timeout);

    unsigned int connect_timeout = 1;
    mysql_options(&m_mysql, MYSQL_OPT_CONNECT_TIMEOUT, &connect_timeout);

    uint64_t auto_reconnect = 1;
    mysql_options(&m_mysql, MYSQL_OPT_RECONNECT, &auto_reconnect);

    bool ssl = false;
    mysql_options(&m_mysql, MYSQL_OPT_SSL_ENFORCE, &ssl);

    //  Some mysql libraries do not support SSL_MODE_DISABLED
#ifdef WING_PERCONA_SSL_DISABLED
    uint32_t ssl_mode = SSL_MODE_DISABLED;
    mysql_options(&m_mysql, MYSQL_OPT_SSL_MODE, &ssl_mode);
#endif
}

QueryHandle::~QueryHandle()
{
    Reset();
    mysql_close(&m_mysql);
}

auto QueryHandle::Reset() -> void {
    freeResult();
    m_statement = Statement();
    m_query_status = QueryStatus::BUILDING;
    m_user_data = nullptr;
    m_had_error = false;
}

auto QueryHandle::SetOnCompleteHandler(
    std::function<void(Query)> on_complete
) -> void
{
    m_on_complete = std::move(on_complete);
}

auto QueryHandle::GetQueryStatus() const -> QueryStatus
{
    return m_query_status;
}

auto QueryHandle::SetTimeout(
    std::chrono::milliseconds timeout
) -> void
{
    auto timeout_seconds = std::chrono::duration_cast<std::chrono::seconds>(m_timeout);
    unsigned int read_timeout = static_cast<unsigned int>(timeout_seconds.count());
    mysql_options(&m_mysql, MYSQL_OPT_READ_TIMEOUT, &read_timeout);

    m_timeout = timeout;
}

auto QueryHandle::GetTimeout() const -> std::chrono::milliseconds
{
    return m_timeout;
}

auto QueryHandle::SetStatement(Statement statement) -> void
{
    m_statement = std::move(statement);
}

auto QueryHandle::GetQueryOriginal() const -> const std::string& {
    return m_final_statement;
}

auto QueryHandle::Execute() -> QueryStatus
{
    if(!m_is_connected)
    {
        if(!connect())
        {
            m_query_status = QueryStatus::CONNECT_FAILURE;
            m_had_error = true;
            return QueryStatus::CONNECT_FAILURE;
        }
    }

    freeResult();

    // ask our statement to prepare the final query string
    m_final_statement = m_statement.prepareStatement(
        [this](const std::string& str_value) {
            if(str_value.empty())
            {
                throw std::invalid_argument("Empty statement part passed in to prepareStatement");
            }
            // https://dev.mysql.com/doc/refman/5.7/en/mysql-real-escape-string.html
            std::string buffer;
            buffer.resize(str_value.length() * 2 + 1);

            size_t length = mysql_real_escape_string(&m_mysql, buffer.data(), &str_value.front(), str_value.length());
            buffer.resize(length);

            return buffer;
        }
    );

    if(0 == mysql_real_query(&m_mysql, m_final_statement.c_str(), m_final_statement.length()))
    {
        m_result = mysql_store_result(&m_mysql);
        if(m_result != nullptr)
        {
            m_query_status = QueryStatus::SUCCESS;
            m_field_count  = mysql_num_fields(m_result);
            m_row_count    = mysql_num_rows(m_result);
            parseRows();
        }
        else
        {
            m_query_status = QueryStatus::STORE_FAILURE;
            m_had_error = true;
        }
    }
    else
    {
        m_query_status = QueryStatus::TIMEOUT;
        m_had_error = true;

    }
    return m_query_status;
}

auto QueryHandle::HasError() const -> bool
{
    return m_had_error;
}

auto QueryHandle::GetError() -> std::string
{
    return mysql_error(&m_mysql);
}

auto QueryHandle::GetFieldCount() const -> size_t
{
    return m_field_count;
}

auto QueryHandle::GetRowCount() const -> size_t
{
    return m_row_count;
}

auto QueryHandle::GetRow(size_t idx) const -> const Row&
{
    return m_rows.at(idx);
}

auto QueryHandle::SetUserData(void* user_data) -> void
{
    m_user_data = user_data;
}

auto QueryHandle::GetUserData() const -> void*
{
    return m_user_data;
}

auto QueryHandle::connect() -> bool
{
    if(!m_is_connected)
    {
        auto* success = mysql_real_connect(
            &m_mysql,
            m_connection.GetHost().c_str(),
            m_connection.GetUser().c_str(),
            m_connection.GetPassword().c_str(),
            m_connection.GetDatabase().c_str(),
            m_connection.GetPort(),
            m_connection.GetSocket().c_str(),
            m_connection.GetClientFlags()
        );

        if (success == nullptr)
        {
            return false;
        }

        m_is_connected = true;
    }

    return true;
}

auto QueryHandle::parseRows() -> void
{
    if(!m_parsed_result && GetRowCount() > 0) {
        m_parsed_result = true;
        m_rows.reserve(GetRowCount());
        MYSQL_ROW mysql_row = nullptr;
        while ((mysql_row = mysql_fetch_row(m_result)))
        {
            auto lengths = mysql_fetch_lengths(m_result);
            Row row(mysql_row, m_field_count, lengths);
            m_rows.emplace_back(std::move(row));
        }
    }
}

auto QueryHandle::freeResult() -> void
{
    if(m_result != nullptr)
    {
        mysql_free_result(m_result);
        m_parsed_result = false;
        m_field_count = 0;
        m_row_count = 0;
        m_rows.clear();
        m_result = nullptr;
    }
}

} // wing
