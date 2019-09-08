cmake_minimum_required(VERSION 2.8)
project(libwingmysql_examples)

message("${PROJECT_NAME} MYSQL_CLIENT_INCLUDE = ${MYSQL_CLIENT_INCLUDE}")
message("${PROJECT_NAME} MYSQL_CLIENT_LIB = ${MYSQL_CLIENT_LIB}")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/${CMAKE_BUILD_TYPE}/bin)

### synch_simple ###
set(SYNCH_SIMPLE_SOURCE_FILES
    examples/sync_simple.cpp
)
add_executable(wing_sync_simple ${SYNCH_SIMPLE_SOURCE_FILES})
target_include_directories(wing_sync_simple SYSTEM PUBLIC ${WING_MYSQL_INCLUDE})
target_link_libraries(wing_sync_simple PRIVATE wingmysql ${WING_MYSQL_LIBRARY_DEPENDENCIES} ${WING_SYSTEM_LIBRARY_DEPENDENCIES})
target_compile_features(wing_sync_simple PRIVATE cxx_std_17)

### async_simple ###
set(ASYNC_SIMPLE_SOURCE_FILES
    examples/async_simple.cpp
)
add_executable(wing_async_simple ${ASYNC_SIMPLE_SOURCE_FILES})
target_include_directories(wing_async_simple SYSTEM PUBLIC ${WING_MYSQL_INCLUDE})
target_link_libraries(wing_async_simple PRIVATE wingmysql ${WING_MYSQL_LIBRARY_DEPENDENCIES} ${WING_SYSTEM_LIBRARY_DEPENDENCIES})
target_compile_features(wing_async_simple PRIVATE cxx_std_17)

### async_benchmarket ###
set(ASYNC_BENCHMARKET_SOURCE_FILES
    examples/async_benchmark.cpp
)
add_executable(wing_async_benchmark ${ASYNC_BENCHMARKET_SOURCE_FILES})
target_include_directories(wing_async_benchmark SYSTEM PUBLIC ${WING_MYSQL_INCLUDE})
target_link_libraries(wing_async_benchmark PRIVATE wingmysql ${WING_MYSQL_LIBRARY_DEPENDENCIES} ${WING_SYSTEM_LIBRARY_DEPENDENCIES})
target_compile_features(wing_async_benchmark PRIVATE cxx_std_17)

### bind ###
set(BIND_SOURCE_FILES
    examples/bind.cpp
)
add_executable(wing_bind ${BIND_SOURCE_FILES})
target_include_directories(wing_bind SYSTEM PUBLIC ${WING_MYSQL_INCLUDE})
target_link_libraries(wing_bind PRIVATE wingmysql ${WING_MYSQL_LIBRARY_DEPENDENCIES} ${WING_SYSTEM_LIBRARY_DEPENDENCIES})
target_compile_features(wing_bind PRIVATE cxx_std_17)
