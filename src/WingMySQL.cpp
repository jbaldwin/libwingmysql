#include "wing/WingMySQL.h"

#include <mysql/mysql.h>

namespace wing
{

auto startup() -> void
{
    mysql_library_init(0, nullptr, nullptr);
}

auto shutdown() -> void
{
    mysql_library_end();
}

} // wing
