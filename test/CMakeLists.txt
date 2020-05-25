cmake_minimum_required(VERSION 3.0)
project(libwingmysql_tests CXX)

option(WING_LOCALHOST_TESTS "Define ON if running tests locally, Default=OFF." OFF)

SET(LIBPWINGMYSQL_TEST_SOURCE_FILES
    TableTest.hpp
)

add_executable(${PROJECT_NAME} main.cpp ${LIBPWINGMYSQL_TEST_SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE wingmysql)

message("${PROJECT_NAME} WING_LOCALHOST_TESTS = ${WING_LOCALHOST_TESTS}")
if(WING_LOCALHOST_TESTS)
    target_compile_definitions(${PROJECT_NAME} PRIVATE WING_LOCALHOST_TESTS)
endif()

if(WING_CODE_COVERAGE)
    target_compile_options(${PROJECT_NAME} PRIVATE --coverage)
    target_link_libraries(${PROJECT_NAME} PRIVATE gcov)
endif()

add_test(NAME WingMySQLTest COMMAND ${PROJECT_NAME})
