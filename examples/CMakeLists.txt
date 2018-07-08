cmake_minimum_required(VERSION 2.8)
project(libwingmysql_examples)

message("${PROJECT_NAME} MYSQL_CLIENT_INCLUDE = ${MYSQL_CLIENT_INCLUDE}")
message("${PROJECT_NAME} MYSQL_CLIENT_LIB = ${MYSQL_CLIENT_LIB}")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/${CMAKE_BUILD_TYPE}/bin)

### synch_simple ###
set(SYNCH_SIMPLE_SOURCE_FILES
    examples/sync_simple.cpp
)
add_executable(sync_simple ${SYNCH_SIMPLE_SOURCE_FILES})
target_include_directories(sync_simple SYSTEM PUBLIC ${WING_MYSQL_INCLUDE})
target_link_libraries(sync_simple PRIVATE wingmysql ${WING_MYSQL_LIBRARY_DEPENDENCIES} ${WING_SYSTEM_LIBRARY_DEPENDENCIES})

### async_simple ###
set(ASYNC_SIMPLE_SOURCE_FILES
    examples/async_simple.cpp
)
add_executable(async_simple ${ASYNC_SIMPLE_SOURCE_FILES})
target_include_directories(async_simple SYSTEM PUBLIC ${WING_MYSQL_INCLUDE})
target_link_libraries(async_simple PRIVATE wingmysql ${WING_MYSQL_LIBRARY_DEPENDENCIES} ${WING_SYSTEM_LIBRARY_DEPENDENCIES})

### async_benchmarket ###
set(ASYNC_BENCHMARKET_SOURCE_FILES
    examples/async_benchmark.cpp
)
add_executable(async_benchmark ${ASYNC_BENCHMARKET_SOURCE_FILES})
target_include_directories(async_benchmark SYSTEM PUBLIC ${WING_MYSQL_INCLUDE})
target_link_libraries(async_benchmark PRIVATE wingmysql ${WING_MYSQL_LIBRARY_DEPENDENCIES} ${WING_SYSTEM_LIBRARY_DEPENDENCIES})

### bind ###
set(BIND_SOURCE_FILES
    examples/bind.cpp
)
add_executable(bind ${BIND_SOURCE_FILES})
target_include_directories(bind SYSTEM PUBLIC ${WING_MYSQL_INCLUDE})
target_link_libraries(bind PRIVATE wingmysql ${WING_MYSQL_LIBRARY_DEPENDENCIES} ${WING_SYSTEM_LIBRARY_DEPENDENCIES})
