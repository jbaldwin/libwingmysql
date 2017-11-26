#pragma once

#include "wing/Request.h"
#include "wing/RequestHandle.h"

#include <deque>
#include <mutex>
#include <memory>
#include <chrono>

namespace wing
{

class EventLoop;

class RequestPool
{
    friend EventLoop;
    friend Request;
public:
    ~RequestPool();

    RequestPool(const RequestPool&) = delete;                       ///< No copying
    RequestPool(RequestPool&&) = default;                           ///< Can move
    auto operator = (const RequestPool&) -> RequestPool& = delete;  ///< No copy assign
    auto operator = (RequestPool&&) -> RequestPool& = default;      ///< Can move assign

    auto Produce(
        const std::string& query,
        std::chrono::milliseconds timeout
    ) -> Request;
private:
    std::mutex m_lock;
    std::deque<std::unique_ptr<RequestHandle>> m_requests;
    EventLoop* m_event_loop;

    explicit RequestPool(
        EventLoop* event_loop
    );

    auto returnRequest(
        std::unique_ptr<RequestHandle> request_handle
    ) -> void;
};

} // wing
