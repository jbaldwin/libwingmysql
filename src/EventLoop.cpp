#include "wing/EventLoop.h"

namespace wing
{

using namespace std::chrono_literals;

auto uv_close_event_loop_callback(uv_handle_t* handle) -> void;
auto on_uv_poll_callback(uv_poll_t* handle, int status, int events) -> void;
auto on_uv_timeout_callback(uv_timer_t* handle) -> void;
auto requests_accept_for_query_async(uv_async_t* async) -> void;
auto requests_accept_for_connect_async(uv_async_t* async) -> void;

EventLoop::EventLoop(
    std::unique_ptr<IRequestCallback> request_callback,
    const std::string& host,
    uint16_t port,
    const std::string& user,
    const std::string& password,
    const std::string& db,
    uint64_t client_flags
)
    : m_request_pool(this),
      m_is_query_running(false),
      m_is_connect_running(false),
      m_is_stopping(false),
      m_active_query_count(0),
      m_request_callback(std::move(request_callback)),
      m_host(host),
      m_port(port),
      m_user(user),
      m_password(password),
      m_db(db),
      m_client_flags(client_flags),
      m_background_query_thread(),
      m_query_loop(uv_loop_new()),
      m_query_async(),
      m_query_async_closed(false),
      m_pending_query_requests_lock(),
      m_pending_query_requests(),
      m_grabbed_query_requests(),
      m_background_connect_thread(),
      m_connect_loop(uv_loop_new()),
      m_connect_async(),
      m_connect_async_closed(false),
      m_pending_connect_requests_lock(),
      m_pending_connect_requests(),
      m_grabbed_connect_requests()
{
    uv_async_init(m_query_loop, &m_query_async, requests_accept_for_query_async);
    m_query_async.data = this;

    uv_async_init(m_connect_loop, &m_connect_async, requests_accept_for_connect_async);
    m_connect_async.data = this;

    m_background_query_thread = std::thread([this] { run_queries(); });
    m_background_connect_thread    = std::thread([this] { run_connect(); });

    while(!IsRunning())
    {
        std::this_thread::sleep_for(1ms);
    }
}

EventLoop::~EventLoop()
{
    if(m_is_query_running && m_is_connect_running)
    {
        Stop();
    }
}

auto EventLoop::IsRunning() -> bool
{
    return m_is_query_running && m_is_connect_running;
}

auto EventLoop::GetActiveQueryCount() const -> uint64_t
{
    return m_active_query_count;
}

auto EventLoop::Stop() -> void
{
    m_is_stopping = true;
    uv_stop(m_query_loop);
    uv_stop(m_connect_loop);
    uv_async_send(&m_query_async);
    uv_async_send(&m_connect_async);

    m_background_query_thread.join();
    m_background_connect_thread.join();

    uv_close(reinterpret_cast<uv_handle_t*>(&m_query_async), uv_close_event_loop_callback);
    uv_close(reinterpret_cast<uv_handle_t*>(&m_connect_async), uv_close_event_loop_callback);

    while(!m_query_async_closed)
    {
        uv_run(m_query_loop, UV_RUN_ONCE);
    }
    while(!m_connect_async_closed)
    {
        uv_run(m_connect_loop, UV_RUN_ONCE);
    }

    m_request_pool.close();

    while(true)
    {
        auto close_retval = uv_loop_close(m_query_loop);
        if(close_retval == 0)
        {
            break;
        }
        else
        {
            uv_run(m_query_loop, UV_RUN_ONCE);
            std::this_thread::sleep_for(1ms);
        }
    }

    while(true)
    {
        auto close_retval = uv_loop_close(m_connect_loop);
        if(close_retval == 0)
        {
            break;
        }
        else
        {
            uv_run(m_connect_loop, UV_RUN_ONCE);
            std::this_thread::sleep_for(1ms);
        }
    }

    /**
     * Clear out any pending / connecting requests.
     */
    {
        std::lock_guard<std::mutex> guard(m_pending_query_requests_lock);
        for(auto& request : m_pending_query_requests)
        {
            request->failed(RequestStatus::SHUTDOWN_IN_PROGRESS);
            (*m_request_callback).OnComplete(std::move(request));
        }
        m_pending_query_requests.clear();
    }

    {
        std::lock_guard<std::mutex> guard(m_pending_connect_requests_lock);
        for(auto& request : m_pending_query_requests)
        {
            request->failed(RequestStatus::SHUTDOWN_IN_PROGRESS);
            (*m_request_callback).OnComplete(std::move(request));
        }
        m_pending_connect_requests.clear();
    }
}

auto EventLoop::GetRequestPool() -> RequestPool&
{
    return m_request_pool;
}

auto EventLoop::StartRequest(Request request) -> bool
{
    // Do not accept new requests if shutting down.
    if(m_is_stopping)
    {
        return false;
    }

    // Do not accept request objects that had errors
    // on previous queries.
    if(request->HasError())
    {
        return false;
    }

    if(request->m_is_connected)
    {
        std::lock_guard<std::mutex> guard(m_pending_query_requests_lock);
        m_pending_query_requests.emplace_back(std::move(request));
        uv_async_send(&m_query_async); // must be in the lock scope
    }
    else
    {
        std::lock_guard<std::mutex> guard(m_pending_connect_requests_lock);
        m_pending_connect_requests.emplace_back(std::move(request));
        uv_async_send(&m_connect_async);
    }

    return true;
}

auto EventLoop::GetRequestCallback() -> IRequestCallback&
{
    return *m_request_callback;
}

auto EventLoop::GetRequestCallback() const -> const IRequestCallback&
{
    return *m_request_callback;
}

auto EventLoop::run_queries() -> void
{
    mysql_thread_init();
    m_is_query_running = true;
    uv_run(m_query_loop, UV_RUN_DEFAULT);
    m_is_query_running = false;
    mysql_thread_end();
}

auto EventLoop::run_connect() -> void
{
    mysql_thread_init();
    m_is_connect_running = true;
    uv_run(m_connect_loop, UV_RUN_DEFAULT);
    m_is_connect_running = false;
    mysql_thread_end();
}

auto EventLoop::onClose(
    uv_handle_t* handle
) -> void
{
    if(handle == reinterpret_cast<uv_handle_t*>(&m_query_async))
    {
        m_query_async_closed = true;
    }
    else if(handle == reinterpret_cast<uv_handle_t*>(&m_connect_async))
    {
        m_connect_async_closed = true;
    }
}

auto EventLoop::onPoll(
    uv_poll_t* handle,
    int /*status*/,
    int events
) -> void
{
    auto* request_handle = static_cast<RequestHandle*>(handle->data);

    switch(static_cast<EventLoop::UVPollEvent>(events)) {
        case UVPollEvent::READABLE: {
            uv_poll_stop(&request_handle->m_poll);

            request_handle->onRead();
            --m_active_query_count;
            // Regardless of the onRead() result the request is done
            (*m_request_callback).OnComplete(
                Request(RequestHandlePtr(request_handle))
            );
        } break;
        case UVPollEvent::WRITEABLE: {
            if(request_handle->onWrite())
            {
                // The write succeeded, now poll for the response.
                uv_poll_start(
                    &request_handle->m_poll,
                    UV_READABLE | UV_DISCONNECT,
                    on_uv_poll_callback
                );
            }
            else
            {
                --m_active_query_count;
                // If writing failed, notify the client.
                (*m_request_callback).OnComplete(
                    Request(RequestHandlePtr(request_handle))
                );
            }
        } break;
        case UVPollEvent::DISCONNECT: {
            request_handle->onDisconnect();
            --m_active_query_count;
            (*m_request_callback).OnComplete(
                Request(RequestHandlePtr(request_handle))
            );
        } break;
        case UVPollEvent::PRIORITIZED: {
            // ignored event type
        } break;
    }
}

auto EventLoop::onTimeout(
    uv_timer_t* handle
) -> void
{
    auto* request_handle = static_cast<RequestHandle*>(handle->data);

    uv_poll_stop(&request_handle->m_poll);
    request_handle->onTimeout();
    --m_active_query_count;
    (*m_request_callback).OnComplete(
        Request(RequestHandlePtr(request_handle))
    );
}

auto EventLoop::requestsAcceptForQueryAsync(
    uv_async_t* /*async*/
) -> void
{
    {
        std::lock_guard<std::mutex> guard(m_pending_query_requests_lock);
        m_grabbed_query_requests.swap(m_pending_query_requests);
    }

    for(auto& request : m_grabbed_query_requests)
    {
        /**
         * If the request had a connect error simply notify the user.
         */
        if(   request->HasError()
           && request->GetRequestStatus() == RequestStatus::CONNECT_FAILURE)
        {
            (*m_request_callback).OnComplete(std::move(request));
            continue;
        }

        auto* request_handle = request.m_request_handle.release();
        request_handle->start();
        uv_poll_start(
            &request_handle->m_poll,
            UV_WRITABLE | UV_DISCONNECT,
            on_uv_poll_callback
        );
    }

    m_active_query_count += m_grabbed_query_requests.size();
    m_grabbed_query_requests.clear();
}

auto EventLoop::requestsAcceptForConnectAsync(
    uv_async_t* /*async*/
) -> void
{
    {
        std::lock_guard<std::mutex> guard(m_pending_connect_requests_lock);
        m_grabbed_connect_requests.swap(m_pending_connect_requests);
    }

    for(auto& request : m_grabbed_connect_requests)
    {
        if(!request->connect())
        {
            request->failed(RequestStatus::CONNECT_FAILURE);
        }
    }

    {
        /**
         * All requests go to the query thread before having their
         * OnComplete() called -- even if they failed to connect.
         * This is done so only 1 thread is ever calling the OnComplete()
         * function at any given time negating the end user from needing
         * to do any sort of locking on their side.
         */
        std::lock_guard<std::mutex> guard(m_pending_query_requests_lock);
        for(auto& request : m_grabbed_connect_requests)
        {
            m_pending_query_requests.emplace_back(std::move(request));
        }
        uv_async_send(&m_query_async); // must be in the lock scope
    }

    m_grabbed_connect_requests.clear();
}

auto uv_close_event_loop_callback(uv_handle_t* handle) -> void
{
    auto event_loop = static_cast<EventLoop*>(handle->data);
    event_loop->onClose(handle);
}

auto on_uv_poll_callback(uv_poll_t* handle, int status, int events) -> void
{
    auto* request_handle = static_cast<RequestHandle*>(handle->data);
    auto* event_loop = request_handle->m_event_loop;
    event_loop->onPoll(handle, status, events);
}

auto on_uv_timeout_callback(uv_timer_t* handle) -> void
{
    auto* request_handle = static_cast<RequestHandle*>(handle->data);
    auto* event_loop = request_handle->m_event_loop;
    event_loop->onTimeout(handle);
}

auto requests_accept_for_query_async(uv_async_t* async) -> void
{
    auto* event_loop = static_cast<EventLoop*>(async->data);
    event_loop->requestsAcceptForQueryAsync(async);
}

auto requests_accept_for_connect_async(uv_async_t* async) -> void
{
    auto* event_loop = static_cast<EventLoop*>(async->data);
    event_loop->requestsAcceptForConnectAsync(async);
}

} // wing
