#include "wing/EventLoop.h"

namespace wing
{

using namespace std::chrono_literals;

auto uv_close_event_loop_callback(
    uv_handle_t* handle
) -> void;

auto on_uv_query_execute_callback(
    uv_work_t* req
) -> void;

auto on_complete_uv_query_execute_callback(
    uv_work_t* req,
    int status
) -> void;

auto requests_accept_for_query_async(
    uv_async_t* async
) -> void;

EventLoop::EventLoop(
    ConnectionInfo connection
)
    :   m_query_pool(std::move(connection), this)
{
    uv_async_init(m_query_loop, &m_query_async, requests_accept_for_query_async);
    m_query_async.data = this;

    m_background_query_thread = std::thread([this] { run_queries(); });

    while(!IsRunning())
    {
        std::this_thread::sleep_for(1ms);
    }
}

EventLoop::~EventLoop()
{
    if(m_is_query_running)
    {
        Stop();
    }
}

auto EventLoop::IsRunning() -> bool
{
    return m_is_query_running;
}

auto EventLoop::GetActiveQueryCount() const -> uint64_t
{
    return m_active_query_count;
}

auto EventLoop::Stop() -> void
{
    m_is_stopping = true;

    uv_stop(m_query_loop);
    uv_async_send(&m_query_async);

    m_background_query_thread.join();

    /**
     * Clear out any pending requests.
     */
    {
        std::lock_guard<std::mutex> guard(m_pending_queries_lock);
        for(auto& query : m_pending_queries)
        {
            query->m_query_status = QueryStatus::SHUTDOWN_IN_PROGRESS;
            query->m_had_error = true;
            callOnComplete(std::move(query));
        }
        m_pending_queries.clear();
    }


    uv_close(reinterpret_cast<uv_handle_t*>(&m_query_async), uv_close_event_loop_callback);

    while(!m_query_async_closed)
    {
        uv_async_send(&m_query_async);
        uv_run(m_query_loop, UV_RUN_ONCE);
    }

    while(true)
    {
        /**
         * Must be done last so the QueryHandles do not destruct before letting the client know
         * the queries are being shutdown.
         */
        m_query_pool.close();
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
}

auto EventLoop::GetQueryPool() -> QueryPool&
{
    return m_query_pool;
}

auto EventLoop::StartQuery(
    Query query
) -> bool
{
    // Do not accept new queries if shutting down.
    if(m_is_stopping)
    {
        return false;
    }

    // Do not accept query objects that had errors
    // on previous queries.
    if(query->HasError())
    {
        return false;
    }

    {
        std::lock_guard<std::mutex> guard(m_pending_queries_lock);
        m_pending_queries.emplace_back(std::move(query));
        uv_async_send(&m_query_async); // must be in the lock scope
    }

    return true;
}

auto EventLoop::run_queries() -> void
{
    mysql_thread_init();
    m_is_query_running = true;
    uv_run(m_query_loop, UV_RUN_DEFAULT);
    m_is_query_running = false;
    mysql_thread_end();
}

auto EventLoop::callOnComplete(Query query) -> void {
    auto on_complete = query->m_on_complete;
    on_complete(std::move(query));
}

auto EventLoop::callOnComplete(std::unique_ptr<QueryHandle> query_handle) -> void {
    auto on_complete = query_handle->m_on_complete;
    on_complete(Query(std::move(query_handle)));
}

auto EventLoop::onClose(
    uv_handle_t* handle
) -> void
{
    if(handle == reinterpret_cast<uv_handle_t*>(&m_query_async))
    {
        m_query_async_closed = true;
    }
}

auto EventLoop::requestsAcceptForQueryAsync(
    uv_async_t* /*async*/
) -> void
{
    {
        std::lock_guard<std::mutex> guard(m_pending_queries_lock);
        m_grabbed_queries.swap(m_pending_queries);
    }

    for(auto& query : m_grabbed_queries)
    {
        /**
         * This is moving ownership into libuv while the query is executed.
         */
        QueryHandle* query_handle = query.m_query_handle.release();

        auto* work = new uv_work_t();
        work->data = query_handle;
        ++m_active_query_count;
        uv_queue_work(
            m_query_loop,
            work,
            on_uv_query_execute_callback,
            on_complete_uv_query_execute_callback
        );
    }

    m_grabbed_queries.clear();
}

auto uv_close_event_loop_callback(
    uv_handle_t* handle
) -> void
{
    auto event_loop = static_cast<EventLoop*>(handle->data);
    event_loop->onClose(handle);
}

auto on_uv_query_execute_callback(
    uv_work_t* req
) -> void
{
    /**
     * This function runs on an background libuv worker thread.
     */
    auto* query_handle = static_cast<QueryHandle*>(req->data);
    query_handle->Execute();
}

auto on_complete_uv_query_execute_callback(
    uv_work_t* req,
    int /*status*/
) -> void
{
    /**
     * This function runs on the libuv query event loop thread.
     */

    // Regain ownership of the QueryHandle from libuv.
    std::unique_ptr<QueryHandle> query_handle_ptr(
        static_cast<QueryHandle*>(req->data)
    );
    auto* event_loop = query_handle_ptr->m_event_loop;
    --event_loop->m_active_query_count;
    // Regardless of the onRead() result the request is done
    event_loop->callOnComplete(std::move(query_handle_ptr));

    delete req;
}

auto requests_accept_for_query_async(
    uv_async_t* async
) -> void
{
    auto* event_loop = static_cast<EventLoop*>(async->data);
    event_loop->requestsAcceptForQueryAsync(async);
}

} // wing
