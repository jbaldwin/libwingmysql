#pragma once

#include "catch.hpp"

#include <wing/WingMySQL.hpp>

#include <chrono>

TEST_CASE("Create table")
{
    using namespace std::chrono_literals;
    wing::ConnectionInfo connection { "mysql", 3306, "user", "passw0rd" };
    wing::Executor executor { std::move(connection) };

    wing::Statement create_table_stm {};
    create_table_stm.ExpectResult(false);
    create_table_stm << "CREATE TABLE IF NOT EXISTS test_db.test_table(a int, b int)";

    auto query1 = executor.StartQuery(std::move(create_table_stm), 10s).value().get();

    CHECK(!query1->Error().has_value());
    CHECK(query1->QueryStatus() == wing::QueryStatus::SUCCESS);

    wing::Statement drop_table_stm {};
    drop_table_stm.ExpectResult(false);
    drop_table_stm << "DROP TABLE test_db.test_table";

    auto query2 = executor.StartQuery(std::move(drop_table_stm), 10s).value().get();
    CHECK(query2->QueryStatus() == wing::QueryStatus::SUCCESS);
}
