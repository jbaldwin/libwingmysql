#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <wing/WingMySQL.hpp>

static wing::GlobalScopeInitializer g_wing_gsi{};

TEST_CASE("catch_is_working")
{
    CHECK(true);
}

#include "TableTest.hpp"
