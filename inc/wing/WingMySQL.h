#pragma once

#include "wing/ConnectionInfo.h"
#include "wing/EventLoop.h"
#include "wing/Query.h"
#include "wing/QueryHandle.h"
#include "wing/QueryPool.h"
#include "wing/QueryStatus.h"

namespace wing
{

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

} // wing
