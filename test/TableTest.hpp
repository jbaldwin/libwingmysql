#pragma once

#include "catch.hpp"

#include <wing/WingMySQL.hpp>

#include <chrono>

TEST_CASE("Create table")
{
    using namespace std::chrono_literals;
    wing::ConnectionInfo connection { MYSQL_HOSTNAME, MYSQL_PORT, MYSQL_USERNAME, MYSQL_PASSWORD };

    REQUIRE(connection.Host() == MYSQL_HOSTNAME);
    REQUIRE(connection.Port() == MYSQL_PORT);
    REQUIRE(connection.User() == MYSQL_USERNAME);
    REQUIRE(connection.Password() == MYSQL_PASSWORD);
    REQUIRE(connection.Database() == "");
    REQUIRE(connection.ClientFlags() == 0);

    wing::Executor executor { std::move(connection) };

    wing::Statement create_table_stm {};
    create_table_stm.ExpectResult(false);
    create_table_stm << "CREATE TABLE IF NOT EXISTS wing_db.test_table(a int, b int)";

    auto query1 = executor.StartQuery(std::move(create_table_stm), 10s).value().get();

    if (query1->Error().has_value()) {
        std::cerr << query1->Error().value() << "\n";
    }
    CHECK(query1->QueryStatus() == wing::QueryStatus::SUCCESS);
    REQUIRE(!query1->Error().has_value());

    wing::Statement drop_table_stm {};
    drop_table_stm.ExpectResult(false);
    drop_table_stm << "DROP TABLE wing_db.test_table";

    auto query2 = executor.StartQuery(std::move(drop_table_stm), 10s).value().get();
    REQUIRE(query2->QueryStatus() == wing::QueryStatus::SUCCESS);
}
