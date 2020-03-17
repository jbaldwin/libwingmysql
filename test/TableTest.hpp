#pragma once

#include "catch.hpp"

#include <wing/WingMySQL.hpp>

#include <chrono>

TEST_CASE("Create table")
{
    using namespace std::chrono_literals;
    wing::ConnectionInfo connection{ "127.0.0.1", 3306, "user", "passw0rd" };
    wing::QueryPool pool{ std::move(connection) };

    wing::Statement create_table_stm{};
    create_table_stm.ExpectResult(false);
    create_table_stm << "CREATE TABLE IF NOT EXISTS test_db.test_table(a int, b int)";

    auto query = pool.Produce(std::move(create_table_stm), 10s);
    CHECK(!query->Error().has_value());
    CHECK(query->Execute() == wing::QueryStatus::SUCCESS);

    wing::Statement drop_table_stm{};
    drop_table_stm.ExpectResult(false);
    drop_table_stm << "DROP TABLE test_db.test_table";

    query = pool.Produce(std::move(drop_table_stm), 10s);
    CHECK(query->Execute() == wing::QueryStatus::SUCCESS);
}
