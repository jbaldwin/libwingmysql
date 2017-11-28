cmake_minimum_required(VERSION 2.8)
project(libwingmysql_examples)

message("MYSQL_CLIENT (examples) = ${MYSQL_CLIENT}")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

### asynch_simple ###
set(ASYNCH_SIMPLE_SOURCE_FILES
    examples/asynch_simple.cpp
)
add_executable(asynch_simple ${ASYNCH_SIMPLE_SOURCE_FILES})
target_link_libraries(asynch_simple PRIVATE wingmysql uv ${MYSQL_CLIENT} pthread m rt dl)

### bind ###
set(BIND_SOURCE_FILES
    examples/bind.cpp
)
add_executable(bind ${BIND_SOURCE_FILES})
target_link_libraries(bind PRIVATE wingmysql uv ${MYSQL_CLIENT} pthread m rt dl)