cmake_minimum_required(VERSION 3.0.2)
project(wingmysql CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(WING_MYSQL_OPT_SSL_ENFORCE_DISABLED "Disable MySQL option SSL enforce" OFF)
option(WING_PERCONA_SSL_DISABLED "Disable Percona MySQL SSL" OFF)

set(WING_MYSQL_CLIENT_INCLUDE "/usr/include/mysql" CACHE PATH "User specified mysql-client include location.")
set(WING_MYSQL_CLIENT_LIB "libmysqlclient_r.so" CACHE STRING "User specified mysql-client location.")
set(WING_SSL_INCLUDE "" CACHE PATH "User specified ssl include location.")
set(WING_LIBSSL "ssl" CACHE STRING "User specified libssl location.")
set(WING_LIBCRYPTO "crypto" CACHE STRING "User specified libcrypto location.")

message("${PROJECT_NAME} WING_MYSQL_CLIENT_INCLUDE = ${WING_MYSQL_CLIENT_INCLUDE}")
message("${PROJECT_NAME} WING_MYSQL_CLIENT_LIB = ${WING_MYSQL_CLIENT_LIB}")
message("${PROJECT_NAME} WING_SSL_INCLUDE = ${WING_SSL_INCLUDE}")
message("${PROJECT_NAME} WING_LIBSSL = ${WING_LIBSSL}")
message("${PROJECT_NAME} WING_LIBSSL = ${WING_LIBSSL}")
message("${PROJECT_NAME} WING_MYSQL_OPT_SSL_ENFORCE_DISABLED = ${WING_MYSQL_OPT_SSL_ENFORCE_DISABLED}")
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
    ${WING_MYSQL_CLIENT_INCLUDE}
    ${WING_SSL_INCLUDE}
)

set(WING_MYSQL_LIBRARY_DEPENDENCIES
    ${WING_MYSQL_CLIENT_LIB}
    ${WING_LIBSSL}
    ${WING_LIBCRYPTO}
)

set(LIB_WING_MYSQL_SOURCE_FILES
    inc/wing/ConnectionInfo.hpp src/ConnectionInfo.cpp
    inc/wing/EventLoop.hpp src/EventLoop.cpp
    inc/wing/QueryHandle.hpp src/QueryHandle.cpp
    inc/wing/Query.hpp src/Query.cpp
    inc/wing/QueryPool.hpp src/QueryPool.cpp
    inc/wing/QueryStatus.hpp src/QueryStatus.cpp
    inc/wing/Row.hpp src/Row.cpp
    inc/wing/Statement.hpp inc/wing/Statement.tcc src/Statement.cpp
    inc/wing/Util.hpp src/Util.cpp
    inc/wing/Value.hpp src/Value.cpp
    inc/wing/WingMySQL.hpp src/WingMySQL.cpp
)

##### Export the Library target #####

add_library(${PROJECT_NAME} STATIC ${LIB_WING_MYSQL_SOURCE_FILES})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${WING_MYSQL_INCLUDE})

target_link_libraries(${PROJECT_NAME} PUBLIC ${WING_MYSQL_LIBRARY_DEPENDENCIES})
target_link_libraries(${PROJECT_NAME} PUBLIC  ${WING_SYSTEM_LIBRARY_DEPENDENCIES})

if (WING_MYSQL_OPT_SSL_ENFORCE_DISABLED)
    target_compile_definitions(${PROJECT_NAME} PUBLIC WING_MYSQL_OPT_SSL_ENFORCE_DISABLED)
ENDIF()

if (WING_PERCONA_SSL_DISABLED)
    target_compile_definitions(${PROJECT_NAME} PUBLIC WING_PERCONA_SSL_DISABLED)
ENDIF()

add_subdirectory(examples)
enable_testing()
add_subdirectory(test)
