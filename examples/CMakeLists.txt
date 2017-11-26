cmake_minimum_required(VERSION 2.8)
project(libwingmysql_examples)

message("MYSQL_CLIENT (examples) = ${MYSQL_CLIENT}")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

### synch_simple ###
set(SYNCH_SIMPLE_SOURCE_FILES
    examples/synch_simple.cpp
)
add_executable(synch_simple ${SYNCH_SIMPLE_SOURCE_FILES})
target_link_libraries(synch_simple PRIVATE wingmysql uv ${MYSQL_CLIENT} pthread m rt dl)
