#include "wing/QueryHandle.h"
#include "wing/EventLoop.h"
#include "wing/QueryPool.h"
#include "wing/Util.h"

#include <stdexcept>
#include <sstream>

namespace wing
{

extern auto on_uv_timeout_callback(uv_timer_t* handle) -> void;

auto on_uv_close_request_handle_callback(
    uv_handle_t* handle
) -> void;

QueryHandle::QueryHandle(
    EventLoop* event_loop,
    QueryPool* query_pool,
    const Connection& connection,
    std::chrono::milliseconds timeout,
    std::string query
)
    : m_event_loop(event_loop),
      m_query_pool(query_pool),
      m_connection(connection),
      m_poll(),
      m_timeout_timer(),
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
      m_original_query(),
      m_query_buffer(),
      m_query_parts(m_bind_param_count),
      m_bind_param_count(0),
      m_bind_params(),
      m_poll_closed(false),
      m_timeout_timer_closed(false),
      m_converter()
{
    mysql_init(&m_mysql);
    // set the mysql timeout in the for synchronous queries...
    // otherwise timeouts are handled through libuv.
    unsigned int read_timeout = static_cast<unsigned int>(m_timeout.count());
    mysql_options(&m_mysql, MYSQL_OPT_READ_TIMEOUT, &read_timeout);
    unsigned int connect_timeout = 1;
    mysql_options(&m_mysql, MYSQL_OPT_CONNECT_TIMEOUT, &connect_timeout);

    SetQuery(std::move(query));
}

QueryHandle::~QueryHandle()
{
    freeResult();
    mysql_close(&m_mysql);
}

auto QueryHandle::GetRequestStatus() const -> QueryStatus
{
    return m_query_status;
}

auto QueryHandle::SetTimeout(std::chrono::milliseconds timeout) -> void
{
    m_timeout = timeout;
}

auto QueryHandle::GetTimeout() const -> std::chrono::milliseconds
{
    return m_timeout;
}

auto QueryHandle::SetQuery(std::string query) -> void
{
    m_original_query = std::move(query);
    // Find all of the bind params.
    m_query_parts = split_view(m_original_query, '?');
    if(m_query_parts.size() > 1)
    {
        m_query_buffer.reserve(m_original_query.size() * 2 + 1);
        m_bind_param_count = m_query_parts.size() - 1;
        m_bind_params.reserve(m_field_count);
    }
    else
    {
        m_field_count = 0;
    }
}

auto QueryHandle::GetQueryOriginal() const -> const std::string&
{
    return m_original_query;
}

auto QueryHandle::GetQueryWithBindParams() const -> const std::string&
{
    return m_query_buffer;
}

auto QueryHandle::BindString(const std::string& param) -> void
{
    // https://dev.mysql.com/doc/refman/5.7/en/mysql-real-escape-string.html
    std::string buffer;
    buffer.resize(param.length() * 2 + 1);

    size_t length = mysql_real_escape_string(&m_mysql, &buffer.front(), &param.front(), param.length());
    buffer.resize(length);

    m_bind_params.emplace_back(std::move(buffer));
}

auto QueryHandle::BindUInt64(uint64_t param) -> void
{
    m_converter.clear();
    m_converter.str("");
    m_converter << param;
    m_bind_params.emplace_back(m_converter.str());
}

auto QueryHandle::BindInt64(int64_t param) -> void
{
    m_converter.clear();
    m_converter.str("");
    m_converter << param;
    m_bind_params.emplace_back(m_converter.str());
}

auto QueryHandle::Execute() -> QueryStatus
{
    if(!m_is_connected)
    {
        if(!connect())
        {
           return QueryStatus::CONNECT_FAILURE;
        }
    }

    freeResult();
    bindParameters();

    const std::string& query = (m_bind_param_count == 0) ? m_original_query : m_query_buffer;
    if(0 == mysql_real_query(&m_mysql, query.c_str(), query.length()))
    {
        m_result = mysql_store_result(&m_mysql);
        if(m_result != nullptr)
        {
            m_query_status = QueryStatus::SUCCESS;
            m_field_count  = mysql_num_fields(m_result);
            m_row_count    = mysql_num_rows(m_result);
        }
        else
        {
            m_query_status = QueryStatus::READ_FAILURE;
        }
    }
    else
    {
        m_query_status = QueryStatus::TIMEOUT;
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

auto QueryHandle::GetRows() const -> const std::vector<Row>&
{
    if(!m_parsed_result) {
        m_parsed_result = true;
        m_rows.reserve(GetRowCount());
        MYSQL_ROW mysql_row = nullptr;
        while ((mysql_row = mysql_fetch_row(m_result)))
        {
            Row row(mysql_row, m_field_count);
            m_rows.emplace_back(std::move(row));
        }
    }

    return m_rows;
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
            "0",
            m_connection.GetClientFlags()
        );

        if (success == nullptr)
        {
            return false;
        }

        // If this request is connected to an
        // event loop, initialize poll/timers.
        if(m_event_loop != nullptr)
        {
            uv_poll_init_socket(m_event_loop->m_query_loop, &m_poll, m_mysql.net.fd);
            m_poll.data = this;

            uv_timer_init(m_event_loop->m_query_loop, &m_timeout_timer);
            m_timeout_timer.data = this;
        }

        m_is_connected = true;
    }

    return true;
}

auto QueryHandle::startAsync() -> void
{
    m_query_status = QueryStatus::EXECUTING;
    freeResult();
    bindParameters();

    uv_timer_start(
        &m_timeout_timer,
        on_uv_timeout_callback,
        static_cast<uint64_t>(m_timeout.count()),
        0
    );
}

auto QueryHandle::bindParameters() -> void
{
    // If there are params to bind, create the true query now.
    if(m_bind_param_count > 0)
    {
        m_query_buffer.clear();
        if(m_bind_param_count != m_bind_params.size())
        {
            throw std::runtime_error("not enough bind params");
        }

        for(size_t i = 0; i < m_query_parts.size(); ++i)
        {
            m_query_buffer.append(m_query_parts[i].to_string());
            // the last query part might be trailing text
            if(i < m_bind_params.size())
            {
                m_query_buffer.append(m_bind_params[i]);
            }
        }
    }
}

auto QueryHandle::failedAsync(QueryStatus status) -> void
{
    m_query_status = status;
    m_had_error = true;
    uv_timer_stop(&m_timeout_timer);
}

auto QueryHandle::onRead() -> bool
{
    auto success = (0 == mysql_read_query_result(&m_mysql));
    if(!success)
    {
        failedAsync(QueryStatus::READ_FAILURE);
        return false;
    }

    m_result = mysql_store_result(&m_mysql);
    if(m_result == nullptr)
    {
        failedAsync(QueryStatus::READ_FAILURE);
        return false;
    }

    uv_timer_stop(&m_timeout_timer);
    m_query_status = QueryStatus::SUCCESS;
    m_field_count    = mysql_num_fields(m_result);
    m_row_count      = mysql_num_rows(m_result);
    return true;
}

auto QueryHandle::onWrite() -> bool
{
    // If there were no bind params, use the raw query provided.
    const std::string& query = (m_bind_param_count == 0) ? m_original_query : m_query_buffer;

    auto success = (0 == mysql_send_query(&m_mysql, query.c_str(), query.length()));
    if(!success)
    {
        failedAsync(QueryStatus::WRITE_FAILURE);
    }
    return success;
}

auto QueryHandle::onTimeout() -> void
{
    failedAsync(QueryStatus::TIMEOUT);
}

auto QueryHandle::onDisconnect() -> void
{
    failedAsync(QueryStatus::DISCONNECT);
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

auto QueryHandle::close() -> void
{
    if(m_event_loop != nullptr)
    {
        uv_close(reinterpret_cast<uv_handle_t*>(&m_poll),          on_uv_close_request_handle_callback);
        uv_close(reinterpret_cast<uv_handle_t*>(&m_timeout_timer), on_uv_close_request_handle_callback);
    }
}

auto on_uv_close_request_handle_callback(uv_handle_t* handle) -> void
{
    auto* request_handle = static_cast<QueryHandle*>(handle->data);

    if(handle == reinterpret_cast<uv_handle_t*>(&request_handle->m_poll))
    {
        request_handle->m_poll_closed = true;
    }
    else if(handle == reinterpret_cast<uv_handle_t*>(&request_handle->m_timeout_timer))
    {
        request_handle->m_timeout_timer_closed = true;
    }

    /**
     * When all uv handles are closed then it is safe to delete the request handle.
     */
    if(   request_handle->m_poll_closed
       && request_handle->m_timeout_timer_closed
    )
    {
        delete request_handle;
    }
}

} // wing
