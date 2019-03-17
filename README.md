libwingmysql - Fast asynchronous C++17 MySQL client.
====================================================

Copyright (c) 2017, Josh Baldwin

https://github.com/jbaldwin/libwingmysql

**libwingmysql** is a C++17 client library that provides and easy to use high throughput asynchronous MySQL queries.

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
    CMake 2.8+
    pthreads/std::thread
    libuv
        [dynamically linked]
        [no override available currently]
    C mysql-client
        [statically linked]
        [override default location via: -DMYSQL_CLIENT:STRING=/usr/lib64/mysql/libmysqlclient_r.a]
        [The _r version is required]

## Instructions

### Building
    # This will produce lib/libwingmysql.a and bin/<examples>
    mkdir Release && cd Release
    cmake -DCMAKE_BUILD_TYPE=Release ..
    cmake --build . --j4 # change 4 to number of cores available

# Examples

See all of the examples under the examples/ directory.

### Simple Synchronous Query
    wing::startup();
    
    wing::ConnectionInfo connection("127.0.0.1", 3306, "user", "password", "DB");
    wing::QueryPool query_pool(connection);
    auto query = query_pool.Produce("SELECT 1");
    if(query->Execute() == wing::QueryStatus::SUCCESS)
    {
        for(auto& row : query->GetRows()
        {
            for(auto& value : row.GetValues())
            {
                // If the field is nullable, use IsNull() to check before using the value.
                // Use As<T> functions to convert the data into the columns type
                std::cout << value.AsStringView() << " ";
            }
            std::cout << "\n";
        }
    }
    
    wing::shutdown();

### Synchronous Query with Bind Parameter
    wing::ConnectionInfo connection("127.0.0.1", 3306, "user", "password", "DB");
    wing::QueryPool query_pool(connection);
    auto query = query_pool.Produce("SELECT ?");
    query->BindUInt64(12345);
    query->Execute();

### Asynchronous Query

See examples/async_simple.cpp

