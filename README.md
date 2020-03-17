libwingmysql - Safe Easy to use C++17 MySQL Client Library
==========================================================

[![CircleCI](https://circleci.com/gh/jbaldwin/libwingmysql/tree/master.svg?style=svg)](https://circleci.com/gh/jbaldwin/libwingmysql/tree/master)
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

## Requirements
    C++17 compiler (g++/clang++)
    CMake
    make and/or ninja
    pthreads/std::thread
    libuv devel
    zlib devel
    mysqlclient devel

## Instructions

### Building
    mkdir Release && cd Release
    cmake -DCMAKE_BUILD_TYPE=Release ..
    cmake --build .

# Examples

See all of the examples under the examples/ directory.

### Simple Synchronous Query
```C++
wing::startup();

wing::ConnectionInfo connection{"127.0.0.1", 3306, "user", "password", "DB"};
wing::QueryPool query_pool{connection};
auto query = query_pool.Produce("SELECT 1");
if(query->Execute() == wing::QueryStatus::SUCCESS)
{
    for(auto& row : query->Rows()
    {
        for(auto& value : row.Values())
        {
            // If the field is nullable, use IsNull() to check before using the value.
            // Use As<T> functions to convert the data into the columns type
            std::cout << value.AsStringView() << " ";
        }
        std::cout << "\n";
    }
}

wing::shutdown();
```

### Synchronous Query with Bind Parameter
```C++
wing::ConnectionInfo connection{"127.0.0.1", 3306, "user", "password", "DB"};
wing::QueryPool query_pool{connection};
auto query = query_pool.Produce("SELECT ?");
query->BindUInt64(12345);
query->Execute();
```

### Asynchronous Query

See examples/async_simple.cpp

## Support

File bug reports, feature requests and questions using [GitHub Issues](https://github.com/jbaldwin/libwingmysql/issues)

Copyright © 2017-2020, Josh Baldwin
