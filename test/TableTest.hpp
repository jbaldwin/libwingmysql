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
    create_table_stm << "CREATE TABLE IF NOT EXISTS " << MYSQL_DATABASE << ".test_table(a int, b int)";

    auto query1 = executor.StartQuery(std::move(create_table_stm), 10s).value().get();
    query_print_error(query1);
    CHECK(query1->QueryStatus() == wing::QueryStatus::SUCCESS);
    REQUIRE(!query1->Error().has_value());

    wing::Statement drop_table_stm {};
    drop_table_stm << "DROP TABLE wing_db.test_table";

    auto query2 = executor.StartQuery(std::move(drop_table_stm), 10s).value().get();
    query_print_error(query2);
    REQUIRE(query2->QueryStatus() == wing::QueryStatus::SUCCESS);
}

TEST_CASE("Insert and select tiny int.")
{
    using namespace std::chrono_literals;
    wing::ConnectionInfo connection { MYSQL_HOSTNAME, MYSQL_PORT, MYSQL_USERNAME, MYSQL_PASSWORD };
    wing::Executor executor { std::move(connection) };

    wing::Statement insert_stm {};
    insert_stm << "INSERT INTO " << MYSQL_DATABASE << ".integers (t, ut) VALUES (-128, 5)";
    auto insert_query = executor.StartQuery(std::move(insert_stm), 10s).value().get();
    query_print_error(insert_query);
    REQUIRE(insert_query->QueryStatus() == wing::QueryStatus::SUCCESS);
    REQUIRE(insert_query->RowCount() == 1);

    wing::Statement select_stm {};
    select_stm << "SELECT t, ut FROM " << MYSQL_DATABASE << ".integers WHERE id = " << insert_query->LastInsertId();
    auto select_query = executor.StartQuery(std::move(select_stm), 10s).value().get();
    query_print_error(select_query);
    REQUIRE(select_query->QueryStatus() == wing::QueryStatus::SUCCESS);
    REQUIRE(select_query->RowCount() == 1);
    const auto& rows = select_query->Rows();
    const auto& values = rows[0].Columns();

    REQUIRE(values[0].AsInt8().has_value());
    REQUIRE(values[0].AsInt8().value() == -128);
    
    REQUIRE(values[1].AsUInt8().has_value());
    REQUIRE(values[1].AsUInt8().value() == 5);

    REQUIRE(values[0].AsStringView() == "-128");
    REQUIRE(values[1].AsStringView() == "5");
}

TEST_CASE("Insert and select small int.")
{
    using namespace std::chrono_literals;
    wing::ConnectionInfo connection { MYSQL_HOSTNAME, MYSQL_PORT, MYSQL_USERNAME, MYSQL_PASSWORD };
    wing::Executor executor { std::move(connection) };

    wing::Statement insert_stm {};
    insert_stm << "INSERT INTO " << MYSQL_DATABASE << ".integers (s, us) VALUES (-32768, 32768)";
    auto insert_query = executor.StartQuery(std::move(insert_stm), 10s).value().get();
    query_print_error(insert_query);
    REQUIRE(insert_query->QueryStatus() == wing::QueryStatus::SUCCESS);
    REQUIRE(insert_query->RowCount() == 1);

    wing::Statement select_stm {};
    select_stm << "SELECT s, us FROM " << MYSQL_DATABASE << ".integers WHERE id = " << insert_query->LastInsertId();
    auto select_query = executor.StartQuery(std::move(select_stm), 10s).value().get();
    query_print_error(select_query);
    REQUIRE(select_query->QueryStatus() == wing::QueryStatus::SUCCESS);
    REQUIRE(select_query->RowCount() == 1);
    const auto& rows = select_query->Rows();
    const auto& values = rows[0].Columns();

    REQUIRE(values[0].AsInt16().has_value());
    REQUIRE(values[0].AsInt16().value() == -32768);
    REQUIRE(values[1].AsUInt16().has_value());
    REQUIRE(values[1].AsUInt16().value() == 32768);

    REQUIRE(values[0].AsStringView() == "-32768");
    REQUIRE(values[1].AsStringView() == "32768");
}

TEST_CASE("Insert and select medium int.")
{
    using namespace std::chrono_literals;
    wing::ConnectionInfo connection { MYSQL_HOSTNAME, MYSQL_PORT, MYSQL_USERNAME, MYSQL_PASSWORD };
    wing::Executor executor { std::move(connection) };

    wing::Statement insert_stm {};
    insert_stm << "INSERT INTO " << MYSQL_DATABASE << ".integers (m, um) VALUES (-8388608, 8388608)";
    auto insert_query = executor.StartQuery(std::move(insert_stm), 10s).value().get();
    query_print_error(insert_query);
    REQUIRE(insert_query->QueryStatus() == wing::QueryStatus::SUCCESS);
    REQUIRE(insert_query->RowCount() == 1);

    wing::Statement select_stm {};
    select_stm << "SELECT m, um FROM " << MYSQL_DATABASE << ".integers WHERE id = " << insert_query->LastInsertId();
    auto select_query = executor.StartQuery(std::move(select_stm), 10s).value().get();
    query_print_error(select_query);
    REQUIRE(select_query->QueryStatus() == wing::QueryStatus::SUCCESS);
    REQUIRE(select_query->RowCount() == 1);
    const auto& rows = select_query->Rows();
    const auto& values = rows[0].Columns();

    REQUIRE(values[0].AsInt32().has_value());
    REQUIRE(values[0].AsInt32().value() == -8388608);

    REQUIRE(values[1].AsUInt32().has_value());
    REQUIRE(values[1].AsUInt32().value() == 8388608);

    REQUIRE(values[0].AsStringView() == "-8388608");
    REQUIRE(values[1].AsStringView() == "8388608");
}

TEST_CASE("Insert and select int.")
{
    using namespace std::chrono_literals;
    wing::ConnectionInfo connection { MYSQL_HOSTNAME, MYSQL_PORT, MYSQL_USERNAME, MYSQL_PASSWORD };
    wing::Executor executor { std::move(connection) };

    wing::Statement insert_stm {};
    insert_stm << "INSERT INTO " << MYSQL_DATABASE << ".integers (i, ui) VALUES (-2147483648, 2147483648)";
    auto insert_query = executor.StartQuery(std::move(insert_stm), 10s).value().get();
    query_print_error(insert_query);
    REQUIRE(insert_query->QueryStatus() == wing::QueryStatus::SUCCESS);
    REQUIRE(insert_query->RowCount() == 1);

    wing::Statement select_stm {};
    select_stm << "SELECT i, ui FROM " << MYSQL_DATABASE << ".integers WHERE id = " << insert_query->LastInsertId();
    auto select_query = executor.StartQuery(std::move(select_stm), 10s).value().get();
    query_print_error(select_query);
    REQUIRE(select_query->QueryStatus() == wing::QueryStatus::SUCCESS);
    REQUIRE(select_query->RowCount() == 1);
    const auto& rows = select_query->Rows();
    const auto& values = rows[0].Columns();
    
    REQUIRE(values[0].AsInt32().has_value());
    REQUIRE(values[0].AsInt32() == -2147483648);
    REQUIRE(values[1].AsUInt32().has_value());
    REQUIRE(values[1].AsUInt32() == 2147483648);

    REQUIRE(values[0].AsStringView() == "-2147483648");
    REQUIRE(values[1].AsStringView() == "2147483648");
}

TEST_CASE("Insert and select bigint.")
{
    using namespace std::chrono_literals;
    wing::ConnectionInfo connection { MYSQL_HOSTNAME, MYSQL_PORT, MYSQL_USERNAME, MYSQL_PASSWORD };
    wing::Executor executor { std::move(connection) };

    wing::Statement insert_stm {};
    insert_stm << "INSERT INTO " << MYSQL_DATABASE << ".integers (b, ub) VALUES (-9223372036854775807, 9223372036854775807)";
    auto insert_query = executor.StartQuery(std::move(insert_stm), 10s).value().get();
    query_print_error(insert_query);
    REQUIRE(insert_query->QueryStatus() == wing::QueryStatus::SUCCESS);
    REQUIRE(insert_query->RowCount() == 1);

    wing::Statement select_stm {};
    select_stm << "SELECT b, ub FROM " << MYSQL_DATABASE << ".integers WHERE id = " << insert_query->LastInsertId();
    auto select_query = executor.StartQuery(std::move(select_stm), 10s).value().get();
    query_print_error(select_query);
    REQUIRE(select_query->QueryStatus() == wing::QueryStatus::SUCCESS);
    REQUIRE(select_query->RowCount() == 1);
    const auto& rows = select_query->Rows();
    const auto& values = rows[0].Columns();

    REQUIRE(values[0].AsInt64().has_value());
    REQUIRE(values[0].AsInt64().value() == -9223372036854775807);
    REQUIRE(values[1].AsInt64().has_value());
    REQUIRE(values[1].AsUInt64().value() == 9223372036854775807);

    REQUIRE(values[0].AsStringView() == "-9223372036854775807");
    REQUIRE(values[1].AsStringView() == "9223372036854775807");
}

TEST_CASE("Test Null Insert")
{
    using namespace std::chrono_literals;
    wing::ConnectionInfo connection { MYSQL_HOSTNAME, MYSQL_PORT, MYSQL_USERNAME, MYSQL_PASSWORD };
    wing::Executor executor { std::move(connection) };

    wing::Statement insert_stm {};
    insert_stm << "INSERT INTO " << MYSQL_DATABASE << ".integers (b, ub) VALUES (NULL, 1515151515)";
    auto insert_query = executor.StartQuery(std::move(insert_stm), 10s).value().get();
    query_print_error(insert_query);
    REQUIRE(insert_query->QueryStatus() == wing::QueryStatus::SUCCESS);
    REQUIRE(insert_query->RowCount() == 1);

    wing::Statement select_stm {};
    select_stm << "SELECT b, ub FROM " << MYSQL_DATABASE << ".integers WHERE id = " << insert_query->LastInsertId();
    auto select_query = executor.StartQuery(std::move(select_stm), 10s).value().get();
    query_print_error(select_query);
    REQUIRE(select_query->QueryStatus() == wing::QueryStatus::SUCCESS);
    REQUIRE(select_query->RowCount() == 1);
    const auto& rows = select_query->Rows();
    const auto& values = rows[0].Columns();

    REQUIRE_FALSE(values[0].AsInt64().has_value());
    REQUIRE(values[1].AsInt64().has_value());
}
