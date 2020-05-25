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

struct TestSetupInfo {
    TestSetupInfo()
    {
        std::cout << "MYSQL_HOSTNAME = " << MYSQL_HOSTNAME << "\n";
        std::cout << "MYSQL_PORT     = " << MYSQL_PORT << "\n";
        std::cout << "MYSQL_USERNAME = " << MYSQL_USERNAME << "\n";
        std::cout << "MYSQL_PASSWORD = " << MYSQL_PASSWORD << "\n";

        // create the test db if it isn't setup by the test harness.
        wing::ConnectionInfo connection { MYSQL_HOSTNAME, MYSQL_PORT, MYSQL_USERNAME, MYSQL_PASSWORD };
        wing::Executor executor { std::move(connection) };
        wing::Statement create_db_stm {};
        create_db_stm << "CREATE DATABASE IF NOT EXISTS " << MYSQL_DATABASE;

        wing::QueryHandle result = executor.StartQuery(std::move(create_db_stm), std::chrono::seconds { 10 }).value().get();
        if (result->Error().has_value()) {
            std::cout << result->Error().value();
        }
    }
} test_setup_info_instance;

#include "TableTest.hpp"
