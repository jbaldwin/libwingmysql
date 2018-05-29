cmake_minimum_required(VERSION 2.8)
project(wingmysql CXX)

message(${CMAKE_CXX_COMPILER_ID})

option(WING_PERCONA_SSL_DISABLED "Disable Percona MySQL SSL" OFF)

set(MYSQL_CLIENT_LIB "libmysqlclient_r.so" CACHE STRING "User specified mysql-client location.")
message("MYSQL_CLIENT_LIB = ${MYSQL_CLIENT_LIB}")

set(MYSQL_CLIENT_INCLUDE "/usr/include/mysql" CACHE STRING "User specified mysql-client include location.")
message("MYSQL_CLIENT_INCLUDE = ${MYSQL_CLIENT_INCLUDE}")

set(SSL_INCLUDE "" CACHE STRING "User specified ssl include location.")
message("SSL_INCLUDE = ${SSL_INCLUDE}")

set(SSL_LIB "ssl" CACHE STRING "User specified libssl location.")
message("SSL_LIB = ${SSL_LIB}")

set(CRYPTO_LIB "crypto" CACHE STRING "User specified libcrypto location.")
message("CRYPTO_LIB = ${CRYPTO_LIB}")

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_BUILD_TYPE}/lib)

if(${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas")
ELSEIF(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded -Wno-weak-vtables -Wno-gnu-anonymous-struct -Wno-nested-anon-types -Wno-exit-time-destructors -Wno-global-constructors -Wno-error=deprecated-declarations -Wno-disabled-macro-expansion -Wno-covered-switch-default -fcolor-diagnostics -ferror-limit=128")
ELSE()
    message(FATAL_ERROR "Unsupported compiler ${CMAKE_CXX_COMPILER}")
ENDIF()

set(CMAKE_CXX_FLAGS "-std=c++17 -Wall -Wextra -Weffc++ -Werror -Wpedantic -pedantic-errors -MD ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -gdwarf-2 -D_GLIBCXX_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -g -gdwarf-2")

if (WING_PERCONA_SSL_DISABLED)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWING_PERCONA_SSL_DISABLED")
ENDIF()

set(LIB_WING_MYSQL_SOURCE_FILES
    inc/wing/ConnectionInfo.h src/ConnectionInfo.cpp
    inc/wing/EventLoop.h src/EventLoop.cpp
    inc/wing/Query.h src/Query.cpp
    inc/wing/QueryHandle.h src/QueryHandle.cpp
    inc/wing/QueryPool.h src/QueryPool.cpp
    inc/wing/QueryStatus.h src/QueryStatus.cpp
    inc/wing/Row.h src/Row.cpp
    inc/wing/Statement.h inc/wing/Statement.tcc src/Statement.cpp
    inc/wing/Util.h src/Util.cpp
    inc/wing/Value.h src/Value.cpp
    inc/wing/WingMySQL.h src/WingMySQL.cpp
)

add_library(${PROJECT_NAME} STATIC ${LIB_WING_MYSQL_SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${MYSQL_CLIENT_INCLUDE} ${SSL_INCLUDE})

target_link_libraries(${PROJECT_NAME} PRIVATE
    uv
    ${MYSQL_CLIENT_LIB}
    pthread
    m
    rt
    dl
    z
    ${SSL_LIB}
    ${CRYPTO_LIB}
)

include(examples/CMakeLists.txt)
