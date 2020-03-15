cmake_minimum_required(VERSION 2.8)
project(libwingmysql_tests CXX)

SET(LIBPRIAMCQL_TEST_SOURCE_FILES
    
)

add_executable(${PROJECT_NAME} main.cpp ${LIBPRIAMCQL_TEST_SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE wingmysql)
add_test(NAME WingMySQLTest COMMAND ${PROJECT_NAME})
