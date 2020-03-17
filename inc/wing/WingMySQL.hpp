#pragma once

#include "wing/ConnectionInfo.hpp"
#include "wing/EventLoop.hpp"
#include "wing/Query.hpp"
#include "wing/QueryHandle.hpp"
#include "wing/QueryPool.hpp"
#include "wing/QueryStatus.hpp"

namespace wing {

/**
 * Starts up libwingmysql.  This must be called
 * once per application before using the library.
 */
auto startup() -> void;

/**
 * Starts up a thread for libwingmysql usage.  This should
 * be called on each thread of the application before it uses
 * any libwingmysql functions.
 */
auto startup_thread() -> void;

/**
 * Shuts down a thread for libwingmysql usage.  This should
 * be called on each thread of the application that used libwingmysql
 * before terminating.
 */
auto shutdown_thread() -> void;

/**
 * Shuts down libwingmysql.  This should be called
 * once per application on shutdown.
 */
auto shutdown() -> void;

/**
 * Use this class at the beginning of main() instead of directly
 * calling startup() and shutdown() to get a nice RAII behavior.
 *
 * e.g:
 *
 * int main()
 * {
 *      wing::GlobalScopeInitializer g_wing_gsi{};
 *
 *      ... code ...
 * }
 *
 */
struct GlobalScopeInitializer {
    GlobalScopeInitializer();
    ~GlobalScopeInitializer();

    GlobalScopeInitializer(const GlobalScopeInitializer&) = delete;
    GlobalScopeInitializer(GlobalScopeInitializer&&) = delete;
    auto operator=(const GlobalScopeInitializer&) -> GlobalScopeInitializer& = delete;
    auto operator=(GlobalScopeInitializer &&) -> GlobalScopeInitializer& = delete;
};

/**
 * Use this class at the beginning of a std::thread instead of directly
 * calling startup_thread() and shutdown_thread() to get nice RAII behavior.
 * 
 * e.g.:
 * 
 * std::thread([]() { 
 *      wing::ThreadScopeInitializer t_wing_tsi{};
 * });
 */
struct ThreadScopeInitializer {
    ThreadScopeInitializer();
    ~ThreadScopeInitializer();

    ThreadScopeInitializer(const ThreadScopeInitializer&) = delete;
    ThreadScopeInitializer(ThreadScopeInitializer&&) = delete;
    auto operator=(const ThreadScopeInitializer&) -> ThreadScopeInitializer& = delete;
    auto operator=(ThreadScopeInitializer &&) -> ThreadScopeInitializer& = delete;
};

} // wing
