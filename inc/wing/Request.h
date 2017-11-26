#pragma once

#include "wing/RequestHandle.h"

#include <memory>

namespace wing
{

class Request
{
    friend class RequestPool;
    friend class EventLoop;
public:
    ~Request();
    Request(const Request&) = delete;                   ///< No copying
    Request(Request&& from) = default;                  ///< Can move
    auto operator = (const Request&) = delete;          ///< No copy assign
    auto operator = (Request&&) -> Request& = default;  ///< Can move assign

    auto operator * () -> RequestHandle&;
    auto operator * () const -> const RequestHandle&;
    auto operator -> () -> RequestHandle*;
    auto operator -> () const -> const RequestHandle*;
private:
    Request(
        std::unique_ptr<RequestHandle> request_handle
    );

    std::unique_ptr<RequestHandle> m_request_handle;
};

} // wing
