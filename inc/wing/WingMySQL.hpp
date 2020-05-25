#pragma once

#include "wing/ConnectionInfo.hpp"
#include "wing/Executor.hpp"
#include "wing/Query.hpp"
#include "wing/QueryHandle.hpp"
#include "wing/QueryPool.hpp"
#include "wing/QueryStatus.hpp"

namespace wing {
class GlobalScopeInitializer {
public:
    GlobalScopeInitializer()
    {
        mysql_library_init(0, nullptr, nullptr);
    }
    ~GlobalScopeInitializer()
    {
        mysql_library_end();
    }
};
} // namespace wing
