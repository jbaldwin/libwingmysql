#pragma once

#include "wing/RequestPool.h"
#include "wing/IRequestCallback.h"

#include <uv.h>

#include <thread>
#include <vector>
#include <memory>
#include <atomic>
#include <mutex>

namespace wing
{

class EventLoop
{
    friend RequestHandle;
    friend RequestPool;
    friend Request;

public:
    EventLoop(
        std::unique_ptr<IRequestCallback> request_callback,
        const std::string& host,
        uint16_t port,
        const std::string& user,
        const std::string& password,
        const std::string& db,
        uint64_t client_flags
    );

    ~EventLoop();

    EventLoop(const EventLoop& copy) = delete;                              ///< No copying
    EventLoop(EventLoop&& move) = default;                                  ///< Can move
    auto operator = (const EventLoop& copy_assign) -> EventLoop& = delete;  ///< No copy assign
    auto operator = (EventLoop&& move_assign) -> EventLoop& = default;      ///< Can move assign

    auto IsRunning() -> bool;

    auto GetActiveQueryCount() const -> uint64_t;

    auto Stop() -> void;

    auto GetRequestPool() -> RequestPool&;

    auto StartRequest(Request request) -> bool;

    auto GetRequestCallback() -> IRequestCallback&;
    auto GetRequestCallback() const -> const IRequestCallback&;

private:
    RequestPool m_request_pool;

    std::atomic<bool> m_is_query_running;
    std::atomic<bool> m_is_connect_running;
    std::atomic<bool> m_is_stopping;
    std::atomic<uint64_t> m_active_query_count;

    std::unique_ptr<IRequestCallback> m_request_callback;

    std::string m_host;
    uint16_t m_port;
    std::string m_user;
    std::string m_password;
    std::string m_db;
    uint64_t m_client_flags;

    std::thread m_background_query_thread;
    uv_loop_t* m_query_loop;
    uv_async_t m_query_async;
    std::atomic<bool> m_query_async_closed;
    std::mutex m_pending_query_requests_lock;
    std::vector<Request> m_pending_query_requests;
    std::vector<Request> m_grabbed_query_requests;

    std::thread m_background_connect_thread;
    uv_loop_t* m_connect_loop;
    uv_async_t m_connect_async;
    std::atomic<bool> m_connect_async_closed;
    std::mutex m_pending_connect_requests_lock;
    std::vector<Request> m_pending_connect_requests;
    std::vector<Request> m_grabbed_connect_requests;

    auto run_queries() -> void;
    auto run_connect() -> void;

    auto onClose(
        uv_handle_t* handle
    ) -> void;

    auto onPoll(
        uv_poll_t* handle,
        int status,
        int events
    ) -> void;

    auto onTimeout(
        uv_timer_t* handle
    ) -> void;

    auto requestsAcceptForQueryAsync(
        uv_async_t* async
    ) -> void;

    auto requestsAcceptForConnectAsync(
        uv_async_t* async
    ) -> void;

    enum class UVPollEvent {
        READABLE    = 1,
        WRITEABLE   = 2,
        DISCONNECT  = 4,
        PRIORITIZED = 8
    };

    friend auto uv_close_event_loop_callback(
        uv_handle_t* handle
    ) -> void;

    friend auto on_uv_poll_callback(
        uv_poll_t* handle,
        int status,
        int events
    ) -> void;

    friend auto on_uv_timeout_callback(
        uv_timer_t* handle
    ) -> void;

    friend auto requests_accept_for_query_async(
        uv_async_t* async
    ) -> void;

    friend auto requests_accept_for_connect_async(
        uv_async_t* async
    ) -> void;

};

} // wing
