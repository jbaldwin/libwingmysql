#include "wing/QueryPool.hpp"
#include "wing/EventLoop.hpp"

namespace wing {

QueryPool::QueryPool(
    ConnectionInfo connection)
    : m_connection(std::move(connection))
{
}

QueryPool::QueryPool(
    ConnectionInfo connection,
    EventLoop* event_loop)
    : m_connection(std::move(connection))
    , m_event_loop(event_loop)
{
}

QueryPool::~QueryPool()
{
    m_queries.clear();
}

auto QueryPool::Produce(
    Statement statement,
    std::chrono::milliseconds timeout) -> QueryHandle
{
    return Produce(std::move(statement), timeout, nullptr);
}

auto QueryPool::Produce(
    Statement statement,
    std::chrono::milliseconds timeout,
    std::function<void(QueryHandle)> on_complete) -> QueryHandle
{
    m_lock.lock();
    if (m_queries.empty()) {
        m_lock.unlock();
        return QueryHandle(
            // Calling new instead of std::make_unique since the ctor is private
            std::unique_ptr<Query>(
                new Query(
                    m_event_loop,
                    *this,
                    m_connection,
                    std::move(on_complete),
                    timeout,
                    std::move(statement))));
    } else {
        auto request_handle_ptr = std::move(m_queries.back());
        m_queries.pop_back();
        m_lock.unlock();

        request_handle_ptr->OnCompleteHandler(std::move(on_complete));
        request_handle_ptr->Timeout(timeout);
        request_handle_ptr->Statement(std::move(statement));

        return QueryHandle(std::move(request_handle_ptr));
    }
}

auto QueryPool::returnQuery(
    std::unique_ptr<Query> query_handle_ptr) -> void
{
    // If the handle has had any kind of error while processing
    // simply release the memory and close it.
    // libuv will shutdown the poll/timer handles
    // and then delete the request handle.
    if (query_handle_ptr->Error().has_value()) {
        return;
    }

    {
        query_handle_ptr->Reset();
        std::lock_guard<std::mutex> guard(m_lock);
        m_queries.emplace_back(std::move(query_handle_ptr));
    }
}

auto QueryPool::close() -> void
{
    std::lock_guard<std::mutex> guard(m_lock);
    m_queries.clear();
}

} // wing
