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
        startup_thread();
    }
}

auto startup_thread() -> void
{
    thread_local std::atomic<uint64_t> initalized{0};

    if(initalized.fetch_add(1) == 0)
    {
        mysql_thread_init();
    }
}

auto shutdown_thread() -> void
{
    thread_local std::atomic<uint64_t> initalized{0};

    if(initalized.fetch_add(1) == 0)
    {
        mysql_thread_end();
    }
}

auto shutdown() -> void
{
    static std::atomic<uint64_t> cleaned{0};

    if(cleaned.fetch_add(1) == 0)
    {
        shutdown_thread();
        mysql_library_end();
    }
}

} // wing
