cmake_minimum_required(VERSION 3.0.2)
project(wingmysql CXX)

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

##### Export the Library target #####

add_library(${PROJECT_NAME} STATIC ${LIB_WING_MYSQL_SOURCE_FILES})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${WING_MYSQL_INCLUDE})

target_link_libraries(${PROJECT_NAME} PUBLIC ${WING_MYSQL_LIBRARY_DEPENDENCIES})
target_link_libraries(${PROJECT_NAME} PUBLIC  ${WING_SYSTEM_LIBRARY_DEPENDENCIES})

add_subdirectory(examples)
enable_testing()
add_subdirectory(test)
