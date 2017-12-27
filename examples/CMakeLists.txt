cmake_minimum_required(VERSION 2.8)
project(libwingmysql_examples)

message("MYSQL_CLIENT_LIB (examples) = ${MYSQL_CLIENT_LIB}")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

### synch_simple ###
set(SYNCH_SIMPLE_SOURCE_FILES
    examples/sync_simple.cpp
)
add_executable(sync_simple ${SYNCH_SIMPLE_SOURCE_FILES})
target_link_libraries(sync_simple PRIVATE wingmysql uv ${MYSQL_CLIENT_LIB} pthread m rt dl ${SSL_LIB} ${CRYPTO_LIB})

### async_simple ###
set(ASYNC_SIMPLE_SOURCE_FILES
    examples/async_simple.cpp
)
add_executable(async_simple ${ASYNC_SIMPLE_SOURCE_FILES})
target_link_libraries(async_simple PRIVATE wingmysql uv ${MYSQL_CLIENT_LIB} pthread m rt dl ${SSL_LIB} ${CRYPTO_LIB})

### async_benchmarket ###
set(ASYNC_BENCHMARKET_SOURCE_FILES
    examples/async_benchmark.cpp
)
add_executable(async_benchmark ${ASYNC_BENCHMARKET_SOURCE_FILES})
target_link_libraries(async_benchmark PRIVATE wingmysql uv ${MYSQL_CLIENT_LIB} pthread m rt dl ${SSL_LIB} ${CRYPTO_LIB})

### bind ###
set(BIND_SOURCE_FILES
    examples/bind.cpp
)
add_executable(bind ${BIND_SOURCE_FILES})
target_link_libraries(bind PRIVATE wingmysql uv ${MYSQL_CLIENT_LIB} pthread m rt dl ${SSL_LIB} ${CRYPTO_LIB})
