#include "wing/RequestHandle.h"
#include "wing/EventLoop.h"
#include "wing/RequestPool.h"

#include <stdexcept>
#include <sstream>

namespace wing
{

extern auto on_uv_timeout_callback(uv_timer_t* handle) -> void;

auto on_uv_close_request_handle_callback(
    uv_handle_t* handle
) -> void;

RequestHandle::RequestHandle(
    EventLoop* event_loop,
    std::chrono::milliseconds timeout,
    std::string query
)
    : m_event_loop(event_loop),
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
      m_request_status(RequestStatus::BUILDING),
      m_query(std::move(query)),
      m_poll_closed(false),
      m_timeout_timer_closed(false)
{

}

RequestHandle::~RequestHandle()
{
    freeResult();
    mysql_close(&m_mysql);
}

auto RequestHandle::GetRequestStatus() const -> RequestStatus
{
    return m_request_status;
}

auto RequestHandle::SetTimeout(std::chrono::milliseconds timeout) -> void
{
    m_timeout = timeout;
}

auto RequestHandle::GetTimeout() const -> std::chrono::milliseconds
{
    return m_timeout;
}

auto RequestHandle::SetQuery(const std::string& query) -> void
{
    m_query = query;
}

auto RequestHandle::GetQuery() const -> const std::string&
{
    return m_query;
}

auto RequestHandle::HasError() const -> bool
{
    return m_had_error;
}

auto RequestHandle::GetError() -> std::string
{
    return mysql_error(&m_mysql);
}

auto RequestHandle::GetFieldCount() const -> size_t
{
    return m_field_count;
}

auto RequestHandle::GetRowCount() const -> size_t
{
    return m_row_count;
}

auto RequestHandle::GetRows() const -> const std::vector<Row>&
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

auto RequestHandle::connect() -> bool
{
    if(!m_is_connected)
    {
        mysql_init(&m_mysql);
        unsigned int connect_timeout = 1;
        mysql_options(&m_mysql, MYSQL_OPT_CONNECT_TIMEOUT, &connect_timeout);

        auto* success = mysql_real_connect(
            &m_mysql,
            m_event_loop->m_host.c_str(),
            m_event_loop->m_user.c_str(),
            m_event_loop->m_password.c_str(),
            m_event_loop->m_db.c_str(),
            m_event_loop->m_port,
            "0",
            m_event_loop->m_client_flags
        );

        if (success == nullptr)
        {
            return false;
        }

        uv_poll_init_socket(m_event_loop->m_query_loop, &m_poll, m_mysql.net.fd);
        m_poll.data = this;

        uv_timer_init(m_event_loop->m_query_loop, &m_timeout_timer);
        m_timeout_timer.data = this;
    }

    m_is_connected = true;
    return true;
}

auto RequestHandle::start() -> void
{
    m_request_status = RequestStatus::EXECUTING;
    uv_timer_start(
        &m_timeout_timer,
        on_uv_timeout_callback,
        static_cast<uint64_t>(m_timeout.count()),
        0
    );
}

auto RequestHandle::failed(RequestStatus status) -> void
{
    m_request_status = status;
    m_had_error = true;
    uv_timer_stop(&m_timeout_timer);
}

auto RequestHandle::onRead() -> bool
{
    auto success = (0 == mysql_read_query_result(&m_mysql));
    if(!success)
    {
        failed(RequestStatus::READ_FAILURE);
        return false;
    }

    m_result = mysql_store_result(&m_mysql);
    if(m_result == nullptr)
    {
        failed(RequestStatus::READ_FAILURE);
        return false;
    }

    uv_timer_stop(&m_timeout_timer);
    m_request_status = RequestStatus::SUCCESS;
    m_field_count    = mysql_num_fields(m_result);
    m_row_count      = mysql_num_rows(m_result);
    return true;
}

auto RequestHandle::onWrite() -> bool
{
    auto success = (0 == mysql_send_query(&m_mysql, m_query.c_str(), m_query.length()));
    if(!success)
    {
        failed(RequestStatus::WRITE_FAILURE);
    }
    return success;
}

auto RequestHandle::onTimeout() -> void
{
    failed(RequestStatus::TIMEOUT);
}

auto RequestHandle::onDisconnect() -> void
{
    failed(RequestStatus::DISCONNECT);
}

auto RequestHandle::freeResult() -> void
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

auto RequestHandle::close() -> void
{
    uv_close(reinterpret_cast<uv_handle_t*>(&m_poll),          on_uv_close_request_handle_callback);
    uv_close(reinterpret_cast<uv_handle_t*>(&m_timeout_timer), on_uv_close_request_handle_callback);
}

auto on_uv_close_request_handle_callback(uv_handle_t* handle) -> void
{
    auto* request_handle = static_cast<RequestHandle*>(handle->data);

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
