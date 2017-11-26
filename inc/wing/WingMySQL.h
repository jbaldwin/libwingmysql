#pragma once

#include "wing/EventLoop.h"
#include "wing/IRequestCallback.h"
#include "wing/Request.h"
#include "wing/RequestHandle.h"
#include "wing/RequestPool.h"
#include "wing/RequestStatus.h"

namespace wing
{

auto startup() -> void;

auto shutdown() -> void;

} // wing
