libwingmysql - Safe Easy to use C++17 MySQL Client Library
==========================================================

[![CI](https://github.com/jbaldwin/libwingmysql/workflows/build-release-test/badge.svg)](https://github.com/jbaldwin/libwingmysql/workflows/build-release-test/badge.svg)
[![Coverage Status](https://coveralls.io/repos/github/jbaldwin/libwingmysql/badge.svg?branch=master)](https://coveralls.io/github/jbaldwin/libwingmysql?branch=master)
[![language][badge.language]][language]
[![license][badge.license]][license]

[badge.language]: https://img.shields.io/badge/language-C%2B%2B17-yellow.svg
[badge.license]: https://img.shields.io/badge/license-Apache--2.0-blue

[language]: https://en.wikipedia.org/wiki/C%2B%2B17
[license]: https://en.wikipedia.org/wiki/Apache_License

https://github.com/jbaldwin/libwingmysql

**libwingmysql** is a C++17 client library that provides and easy to use API for both synchronous _and_ asynchrous MySQL queries.

**libwingmysql** is licensed under the Apache 2.0 license.

# Overview #
* Easy and safe to use C++17 client library API.
* Synchronous and Asynchronous MySQL query support.
* Socket pooling for re-using MySQL connections.  Reduces reconnects.
* EventLoop background query thread for automatically handling inflight asynchronous queries.
* Background connect thread for new sockets -- doesn't block existing in flight queries.
* Completed async queries are notified to the user via simple callback.

# Usage #

# Examples

See all of the examples under the examples/ directory.

### Simple Synchronous Query
```C++
#include <iostream>
#include <wing/WingMySQL.hpp>

int main(int argc, char* argv[])
{
    using namespace std::chrono_literals;
    if (argc < 7) {
        std::cout << argv[0] << " <hostname> <port> <user> <password> <db> <query>\n";
        return 1;
    }

    wing::GlobalScopeInitializer wing_gsi{};

    // Create connection information to the mysql database.
    wing::ConnectionInfo connection {
        argv[1],
        static_cast<uint16_t>(std::stoi(argv[2])),
        argv[3],
        argv[4],
        argv[5],
        0 };

    // Create a statement, normally this would have wing::Statement::Arg()
    // around parameters that should be bound, this is a simple example.
    wing::Statement statement {};
    statement << argv[6];

    // Create an executor with the connection information and then start
    // a query from the statement.  This query is given a timeout of 30s.
    // StartQuery() can fail, and thus returns std::optional, if this holds
    // the std::future<wing::QueryHandle> then the query is executing, wait
    // on the future to complete for the query results.
    wing::Executor executor { std::move(connection) };
    auto query_handle = executor.StartQuery(std::move(statement), 30s).value().get();

    // If the query succeeded print the row contents, all columns can be viewed
    // via AsStringView() but there are As<Type> functions to have wing convert
    // the column value into the expected type for you.
    if (query_handle->QueryStatus() == wing::QueryStatus::SUCCESS) {
        std::cout << "Fields count: " << query_handle->FieldCount() << "\n";
        std::cout << "Row count: " << query_handle->RowCount() << "\n";

        for (const auto& row : query_handle->Rows()) {
            for (const auto& value : row.Columns()) {
                if (value.IsNull()) {
                    std::cout << "NULL ";
                } else {
                    std::cout << value.AsStringView() << " ";
                }
            }
            std::cout << "\n";
        }
    } else {
        std::cout << "An error occurred: " << wing::to_string(query_handle->QueryStatus()) << "\n";
    }

    return 0;
}
```

### Creating a statement with escaped parameters
Wing statements easily support escaping parameters by wrapping them into a
`wing::Statement::Arg`.  If the type passed in is dedeuced to need escaping then wing
will automatically mysql escape the statement argument.

```C++
std::string to_be_escaped = "ESCAPE_ME";
int64_t i_wont_be_escaped = 5;
using Arg = wing::Statement::Arg;
wing::Statement statement {};
statement << "SELECT column FROM DATABASE WHERE name like " << Arg(to_be_escaped) << " AND id = " << Arg(i_wont_be_escaped);
```

## Requirements
    C++17 compiler
        g++-9
        clang-9
    CMake
    make or ninja
    pthreads
    zlib-devel
    mysqlclient-devel

    Test on:
        ubuntu:18.04
        ubuntu:20.04
        fedora:31

## Instructions

### Building
    # This will produce a static library to link against your project.
    mkdir Release && cd Release
    cmake -DCMAKE_BUILD_TYPE=Release ..
    cmake --build .

### CMake Projects
CMake options:

    WING_BUILD_EXAMPLES                  "Build the examples. Default=ON"
    WING_BUILD_TESTS                     "Build the tests. Default=ON"
    WING_CODE_COVERAGE                   "Enable code coverage, tests must also be enabled. Default=OFF"
    WING_MYSQL_OPT_SSL_ENFORCE_DISABLED  "Disable MySQL option SSL enforce. Default=OFF"
    WING_PERCONA_SSL_DISABLED            "Disable Percona MySQL SSL. Default=OFF"
    WING_LINK_LIBRARIES                  "User specified additinoal link targets for custom mysql libraries, defaults to system 'mysqlclient'."

#### add_subdirectory()
To use within your cmake project you can clone the project or use git submodules and then `add_subdirectory` in the paren project's `CMakeLists.txt`, assuming the wing code is in a `libwingmysql/` subdirectory of the parent project:

    add_subdirectory(libwingmysql)

To link to the `<project_name>` then use the following:

    add_executable(<project_name> main.cpp)
    target_link_libraries(<project_name> PUBLIC wingmysql)

Include wing in the project's code by simply including `#include <wing/WingMySQL.hpp>` as needed.

#### FetchContent
CMake can also include the project directly via a `FetchContent` declaration.  In your project's `CMakeLists.txt`
include the following code to download the git repository and make it available to link to.

    cmake_minimum_required(VERSION 3.11)

    # ... cmake project stuff ...

    include(FetchContent)
    FetchContent_Declare(
        wingmysql
        GIT_REPOSITORY https://github.com/jbaldwin/libwingmysql.git
        GIT_TAG        <TAG_OR_GIT_HASH>
    )
    FetchContent_MakeAvailable(wingmysql)

    # ... cmake project more stuff ...

    target_link_libraries(${PROJECT_NAME} PUBLIC wingmysql)

### Running Tests
The tests are automatically run by GitHub Actions on all Pull Requests.  They can also be ran locally with a default
localhost instance of `mysql`.  To do so the CMake option `WING_LOCALHOST_TESTS=ON` must be set otherwise the tests will use the hostname `mysql` setup in the CI settings.  After building and starting `mysql` tests can be run by issuing:

    # Invoke via cmake:
    ctest -v

    # Or invoke directly to see error messages if tests are failing:
    ./test/libwingmysql_tests

## Benchmarks
...TODO...

## Support

File bug reports, feature requests and questions using [GitHub Issues](https://github.com/jbaldwin/libwingmysql/issues)

Copyright © 2017-2020, Josh Baldwin
