#pragma once

#include "wing/ConnectionInfo.h"
#include "wing/EventLoop.h"
#include "wing/IQueryCallback.h"
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
 * Shuts down libwingmysql.  This should be called
 * once per application on shutdown.
 */
auto shutdown() -> void;

} // wing
