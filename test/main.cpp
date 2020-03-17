#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <wing/WingMySQL.hpp>

wing::GlobalScopeInitializer wing_gsi{};

TEST_CASE("catch_is_working")
{
    CHECK(true);
}

#include "TableTest.hpp"