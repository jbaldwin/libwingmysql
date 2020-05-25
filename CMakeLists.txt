cmake_minimum_required(VERSION 3.0)
project(wingmysql CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(WING_BUILD_EXAMPLES                  "Build the examples. Default=ON" ON)
option(WING_BUILD_TESTS                     "Build the tests. Default=ON" ON)
option(WING_CODE_COVERAGE                   "Enable code coverage, tests must also be enabled. Default=OFF" OFF)
option(WING_MYSQL_OPT_SSL_ENFORCE_DISABLED  "Disable MySQL option SSL enforce. Default=OFF" OFF)
option(WING_PERCONA_SSL_DISABLED            "Disable Percona MySQL SSL. Default=OFF" OFF)

set(WING_USER_LINK_LIBRARIES mysqlclient CACHE STRING "User specified additinoal link targets for custom mysql libraries, defaults to system 'mysqlclient'.")

set(WING_LINK_LIBRARIES
    pthread
    m
    rt
    dl
    z
    ${WING_USER_LINK_LIBRARIES}
)

message("${PROJECT_NAME} WING_BUILD_EXAMPLES                    = ${WING_BUILD_EXAMPLES}")
message("${PROJECT_NAME} WING_BUILD_TESTS                       = ${WING_BUILD_TESTS}")
message("${PROJECT_NAME} WING_CODE_COVERAGE                     = ${WING_CODE_COVERAGE}")
message("${PROJECT_NAME} WING_MYSQL_OPT_SSL_ENFORCE_DISABLED    = ${WING_MYSQL_OPT_SSL_ENFORCE_DISABLED}")
message("${PROJECT_NAME} WING_PERCONA_SSL_DISABLED              = ${WING_PERCONA_SSL_DISABLED}")
message("${PROJECT_NAME} WING_LINK_LIBRARIES                    = ${WING_LINK_LIBRARIES}")

set(LIB_WING_MYSQL_SOURCE_FILES
    inc/wing/ConnectionInfo.hpp src/ConnectionInfo.cpp
    inc/wing/Executor.hpp src/Executor.cpp
    inc/wing/QueryHandle.hpp src/QueryHandle.cpp
    inc/wing/Query.hpp src/Query.cpp
    inc/wing/QueryPool.hpp src/QueryPool.cpp
    inc/wing/QueryStatus.hpp src/QueryStatus.cpp
    inc/wing/Row.hpp src/Row.cpp
    inc/wing/Statement.hpp inc/wing/Statement.tcc src/Statement.cpp
    inc/wing/Util.hpp src/Util.cpp
    inc/wing/Value.hpp src/Value.cpp
    inc/wing/WingMySQL.hpp
)

add_library(${PROJECT_NAME} STATIC ${LIB_WING_MYSQL_SOURCE_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_link_libraries(${PROJECT_NAME} PUBLIC ${WING_LINK_LIBRARIES})

if(WING_CODE_COVERAGE)
    target_compile_options(${PROJECT_NAME} PRIVATE --coverage)
    target_link_libraries(${PROJECT_NAME} PRIVATE gcov)
endif()

if (WING_MYSQL_OPT_SSL_ENFORCE_DISABLED)
    target_compile_definitions(${PROJECT_NAME} PUBLIC WING_MYSQL_OPT_SSL_ENFORCE_DISABLED)
ENDIF()

if (WING_PERCONA_SSL_DISABLED)
    target_compile_definitions(${PROJECT_NAME} PUBLIC WING_PERCONA_SSL_DISABLED)
ENDIF()

if(WING_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(WING_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
