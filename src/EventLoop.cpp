#include "wing/EventLoop.h"

namespace wing
{

using namespace std::chrono_literals;

auto uv_close_event_loop_callback(uv_handle_t* handle) -> void;
auto on_uv_poll_callback(uv_poll_t* handle, int status, int events) -> void;
auto on_uv_timeout_callback(uv_timer_t* handle) -> void;
auto requests_accept_async(uv_async_t* async) -> void;

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
      m_is_running(false),
      m_is_stopping(false),
      m_active_request_count(0),
      m_request_callback(std::move(request_callback)),
      m_loop(uv_loop_new()),
      m_async(),
      m_host(host),
      m_port(port),
      m_user(user),
      m_password(password),
      m_db(db),
      m_client_flags(client_flags),
      m_pending_requests_lock(),
      m_pending_requests(),
      m_grabbed_requests(),
      m_background_thread(),
      m_async_closed(false)
{
    uv_async_init(m_loop, &m_async, requests_accept_async);
    m_async.data = this;

    m_background_thread = std::thread([this] { run(); });

    while(!IsRunning())
    {
        std::this_thread::sleep_for(1ms);
    }
}

EventLoop::~EventLoop()
{
    uv_loop_close(m_loop);
}

auto EventLoop::IsRunning() -> bool
{
    return m_is_running;
}

auto EventLoop::GetActiveRequestCount() const -> uint64_t
{
    return m_active_request_count;
}

auto EventLoop::Stop() -> void
{
    m_is_stopping = true;
    uv_close(reinterpret_cast<uv_handle_t*>(&m_async), uv_close_event_loop_callback);
    uv_async_send(&m_async);
    uv_stop(m_loop);

    while(!m_async_closed)
    {
        std::this_thread::sleep_for(1ms);
    }

    m_background_thread.join();
}

auto EventLoop::GetRequestPool() -> RequestPool&
{
    return m_request_pool;
}

auto EventLoop::StartRequest(Request request) -> bool
{
    if(m_is_stopping)
    {
        return false;
    }

    {
        std::lock_guard<std::mutex> guard(m_pending_requests_lock);
        m_pending_requests.emplace_back(std::move(request));
    }
    uv_async_send(&m_async);

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

auto EventLoop::run() -> void
{
    m_is_running = true;
    uv_run(m_loop, UV_RUN_DEFAULT);
    m_is_running = false;
}

auto EventLoop::onClose(
    uv_handle_t* handle
) -> void
{
    if(handle == reinterpret_cast<uv_handle_t*>(&m_async))
    {
        m_async_closed = true;
    }
}

auto EventLoop::onPoll(
    uv_poll_t* handle,
    int /*status*/,
    int events
) -> void
{
    auto* request_handle = static_cast<RequestHandle*>(handle->data);

    uv_poll_stop(&request_handle->m_poll);

    switch(static_cast<EventLoop::UVPollEvent>(events)) {
        case UVPollEvent::READABLE: {
            uv_poll_stop(&request_handle->m_poll);
            request_handle->onRead();
            --m_active_request_count;
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
                --m_active_request_count;
                // If writing failed, notify the client.
                (*m_request_callback).OnComplete(
                    Request(RequestHandlePtr(request_handle))
                );
            }
        } break;
        case UVPollEvent::DISCONNECT: {
            request_handle->onDisconnect();
            --m_active_request_count;
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
    --m_active_request_count;
    (*m_request_callback).OnComplete(
        Request(RequestHandlePtr(request_handle))
    );
}

auto EventLoop::requestsAcceptAsync(
    uv_async_t* /*async*/
) -> void
{
    {
        std::lock_guard<std::mutex> guard(m_pending_requests_lock);
        m_grabbed_requests.swap(m_pending_requests);
    }

    for(auto& request : m_grabbed_requests)
    {
        auto* request_handle = request.m_request_handle.release();
        request_handle->start();
        uv_poll_start(
            &request_handle->m_poll,
            UV_WRITABLE | UV_DISCONNECT,
            on_uv_poll_callback
        );
    }

    m_active_request_count += m_grabbed_requests.size();
    m_grabbed_requests.clear();
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

auto on_uv_timeout_callback(
    uv_timer_t* handle
) -> void
{
    auto* request_handle = static_cast<RequestHandle*>(handle->data);
    auto* event_loop = request_handle->m_event_loop;
    event_loop->onTimeout(handle);
}

auto requests_accept_async(uv_async_t* async) -> void
{
    auto* event_loop = static_cast<EventLoop*>(async->data);
    event_loop->requestsAcceptAsync(async);
}

} // wing
