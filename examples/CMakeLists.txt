cmake_minimum_required(VERSION 3.0)
project(libwingmysql_examples CXX)

### synch_simple ###
add_executable(wing_sync_simple sync_simple.cpp)
target_link_libraries(wing_sync_simple PRIVATE wingmysql)

### async_simple ###
add_executable(wing_async_simple async_simple.cpp)
target_link_libraries(wing_async_simple PRIVATE wingmysql)

### async_benchmarket ###
add_executable(wing_async_benchmark async_benchmark.cpp)
target_link_libraries(wing_async_benchmark PRIVATE wingmysql)

### readme ###
add_executable(wing_readme readme.cpp)
target_link_libraries(wing_readme PRIVATE wingmysql)

### bind ###
add_executable(wing_bind bind.cpp)
target_link_libraries(wing_bind PRIVATE wingmysql)
