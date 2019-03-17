cmake_minimum_required(VERSION 3.0.2)
project(wingmysql CXX)

# Create a Debug/lib or Release/lib output directory.
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_BUILD_TYPE}/lib)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(WING_PERCONA_SSL_DISABLED "Disable Percona MySQL SSL" OFF)

set(MYSQL_CLIENT_INCLUDE "/usr/include/mysql" CACHE PATH "User specified mysql-client include location.")
set(MYSQL_CLIENT_LIB "libmysqlclient_r.so" CACHE STRING "User specified mysql-client location.")
set(SSL_INCLUDE "" CACHE PATH "User specified ssl include location.")
set(SSL_LIB "ssl" CACHE STRING "User specified libssl location.")
set(CRYPTO_LIB "crypto" CACHE STRING "User specified libcrypto location.")

message("${PROJECT_NAME} MYSQL_CLIENT_INCLUDE = ${MYSQL_CLIENT_INCLUDE}")
message("${PROJECT_NAME} MYSQL_CLIENT_LIB = ${MYSQL_CLIENT_LIB}")
message("${PROJECT_NAME} SSL_INCLUDE = ${SSL_INCLUDE}")
message("${PROJECT_NAME} SSL_LIB = ${SSL_LIB}")
message("${PROJECT_NAME} CRYPTO_LIB = ${CRYPTO_LIB}")
message("${PROJECT_NAME} WING_PERCONA_SSL_DISABLED = ${WING_PERCONA_SSL_DISABLED}")

# Set some general system dependencides.
set(WING_SYSTEM_LIBRARY_DEPENDENCIES
    uv
    pthread
    m
    rt
    dl
    z
)

set(WING_MYSQL_INCLUDE
    ${MYSQL_CLIENT_INCLUDE}
    ${SSL_INCLUDE}
)

set(WING_MYSQL_LIBRARY_DEPENDENCIES
    ${MYSQL_CLIENT_LIB}
    ${SSL_LIB}
    ${CRYPTO_LIB}
)

if (WING_PERCONA_SSL_DISABLED)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWING_PERCONA_SSL_DISABLED")
ENDIF()

set(LIB_WING_MYSQL_SOURCE_FILES
    inc/wing/ConnectionInfo.h src/ConnectionInfo.cpp
    inc/wing/EventLoop.h src/EventLoop.cpp
    inc/wing/QueryHandle.h src/QueryHandle.cpp
    inc/wing/Query.h src/Query.cpp
    inc/wing/QueryPool.h src/QueryPool.cpp
    inc/wing/QueryStatus.h src/QueryStatus.cpp
    inc/wing/Row.h src/Row.cpp
    inc/wing/Statement.h inc/wing/Statement.tcc src/Statement.cpp
    inc/wing/Util.h src/Util.cpp
    inc/wing/Value.h src/Value.cpp
    inc/wing/WingMySQL.h src/WingMySQL.cpp
)

add_library(${PROJECT_NAME} STATIC ${LIB_WING_MYSQL_SOURCE_FILES})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${WING_MYSQL_INCLUDE})

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${WING_MYSQL_LIBRARY_DEPENDENCIES}
    ${WING_SYSTEM_LIBRARY_DEPENDENCIES}
)

if(UNIX)
    target_compile_options(
        ${PROJECT_NAME} PRIVATE
            -Wall
            -Wextra
            -Weffc++
            -Werror
            -Wpedantic
            -pedantic-errors
            -MD
    )
endif()

if(${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
    target_compile_options(
        ${PROJECT_NAME} PRIVATE
            -Wno-unknown-pragmas
    )
endif()

if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    target_compile_options(
        ${PROJECT_NAME} PRIVATE
            -Weverything
            -Wno-c++98-compat
            -Wno-c++98-compat-pedantic
            -Wno-padded
            -Wno-weak-vtables
            -Wno-gnu-anonymous-struct
            -Wno-nested-anon-types
            -Wno-exit-time-destructors
            -Wno-global-constructors
            -Wno-error=deprecated-declarations
            -Wno-disabled-macro-expansion
            -Wno-covered-switch-default
            -fcolor-diagnostics
            -ferror-limit=128
    )
endif()

include(examples/CMakeLists.txt)
