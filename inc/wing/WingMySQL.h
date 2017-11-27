#pragma once

#include "wing/EventLoop.h"
#include "wing/IQueryCallback.h"
#include "wing/Query.h"
#include "wing/QueryHandle.h"
#include "wing/QueryPool.h"
#include "wing/QueryStatus.h"

namespace wing
{

auto startup() -> void;

auto shutdown() -> void;

} // wing
