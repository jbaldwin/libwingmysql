#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#ifdef WING_LOCALHOST_TESTS
// Note that using "localhost" makes mysql attempt to use a unix domain socket,
// so explicitly use 127.0.0.1 for "localhost" TCP.
static const std::string MYSQL_HOSTNAME = "127.0.0.1";
static const std::string MYSQL_USERNAME = "root";
static const std::string MYSQL_PASSWORD = "";
#else
static const std::string MYSQL_HOSTNAME = "mysql";
static const std::string MYSQL_USERNAME = "user";
static const std::string MYSQL_PASSWORD = "passw0rd";
#endif

static const uint16_t MYSQL_PORT = 3306;

static const std::string MYSQL_DATABASE = "wing_db";

#include <wing/WingMySQL.hpp>

auto query_print_error(
    wing::QueryHandle& query_handle) -> void
{
    if (query_handle->Error().has_value()) {
        std::cerr << "MySQL Query [" << query_handle->QueryOriginal() << "]\nError: " << query_handle->Error().value() << "\n";
    }
}

struct TestSetupInfo {
    TestSetupInfo()
    {
        using namespace std::chrono_literals;
        std::vector<std::string> table_names {
            "integers_and_char"
        };

        std::cout << "MYSQL_HOSTNAME = " << MYSQL_HOSTNAME << "\n";
        std::cout << "MYSQL_PORT     = " << MYSQL_PORT << "\n";
        std::cout << "MYSQL_USERNAME = " << MYSQL_USERNAME << "\n";
        std::cout << "MYSQL_PASSWORD = " << MYSQL_PASSWORD << "\n";

        // create the test db if it isn't setup by the test harness.
        wing::ConnectionInfo connection { MYSQL_HOSTNAME, MYSQL_PORT, MYSQL_USERNAME, MYSQL_PASSWORD };
        wing::Executor executor { std::move(connection) };
        wing::Statement create_db_stm {};
        create_db_stm << "CREATE DATABASE IF NOT EXISTS " << MYSQL_DATABASE;

        wing::QueryHandle db_result = executor.StartQuery(std::move(create_db_stm), 10s).value().get();
        if (db_result->Error().has_value()) {
            std::cerr << "Failed to setup wing_db database: " << db_result->Error().value() << "\n";
            std::terminate();
        }

        for (const auto& name : table_names) {
            wing::Statement drop_table_stm {};
            drop_table_stm << "DROP TABLE IF EXISTS " << MYSQL_DATABASE << "." << name;
            wing::QueryHandle drop_result = executor.StartQuery(std::move(drop_table_stm), 10s).value().get();
            if (drop_result->Error().has_value()) {
                std::cerr << "Failed to drop table [" << name << "] table: " << drop_result->Error().value() << "\n";
                std::terminate();
            }
        }

        wing::Statement create_table_stm {};
        // clang-format off
        create_table_stm
            << "CREATE TABLE " << MYSQL_DATABASE << ".integers_and_char (\n"
            << "id INT UNSIGNED NOT NULL AUTO_INCREMENT, \n"
            << "t TINYINT, \n"
            << "s SMALLINT, \n"
            << "m MEDIUMINT, \n"
            << "i INT, \n"
            << "b BIGINT, \n"
            << "ut TINYINT UNSIGNED, \n"
            << "us SMALLINT UNSIGNED, \n"
            << "um MEDIUMINT UNSIGNED, \n"
            << "ui INT UNSIGNED, \n"
            << "ub BIGINT UNSIGNED, \n"
            << "vc VARCHAR(10), \n"
            << "PRIMARY KEY (id)\n"
            << ")";
        // clang-format on

        wing::QueryHandle table_result = executor.StartQuery(std::move(create_table_stm), 10s).value().get();
        if (table_result->Error().has_value()) {
            std::cerr << "Failed to setup wing_table table: " << table_result->Error().value() << "\n";
            std::terminate();
        }
    }
} test_setup_info_instance;

#include "TableTest.hpp"
