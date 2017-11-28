#include "wing/WingMySQL.h"

#include <atomic>

#include <mysql/mysql.h>

namespace wing
{

auto startup() -> void
{
    static std::atomic<uint64_t> initialized{0};

    if(initialized.fetch_add(1) == 0)
    {
        mysql_library_init(0, nullptr, nullptr);
    }
}

auto shutdown() -> void
{
    static std::atomic<uint64_t> cleaned{0};

    if(cleaned.fetch_add(1) == 0)
    {
        mysql_library_end();
    }
}

} // wing
