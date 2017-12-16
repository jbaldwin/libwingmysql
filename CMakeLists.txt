cmake_minimum_required(VERSION 2.8)
project(wingmysql CXX)

message(${CMAKE_CXX_COMPILER_ID})

set(MYSQL_CLIENT "/usr/lib64/mysql/libmysqlclient_r.a" CACHE STRING "User specified mysql-client location.")
message("MYSQL_CLIENT = ${MYSQL_CLIENT}")

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)

if(${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas")
ELSEIF(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded -Wno-weak-vtables -Wno-gnu-anonymous-struct -Wno-nested-anon-types -Wno-exit-time-destructors -Wno-global-constructors -Wno-error=deprecated-declarations -Wno-disabled-macro-expansion -Wno-covered-switch-default -fcolor-diagnostics -ferror-limit=128")
ELSE()
    message(FATAL_ERROR "Unsupported compiler ${CMAKE_CXX_COMPILER}")
ENDIF()

set(CMAKE_CXX_FLAGS "-std=c++17 -Wall -Wextra -Weffc++ -Werror -Wpedantic -pedantic-errors -MD ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -gdwarf-2 -D_GLIBCXX_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -g -gdwarf-2")

set(LIB_WING_MYSQL_SOURCE_FILES
    inc/wing/ConnectionInfo.h src/ConnectionInfo.cpp
    inc/wing/EventLoop.h src/EventLoop.cpp
    inc/wing/Query.h src/Query.cpp
    inc/wing/QueryHandle.h src/QueryHandle.cpp
    inc/wing/QueryPool.h src/QueryPool.cpp
    inc/wing/QueryStatus.h src/QueryStatus.cpp
    inc/wing/Row.h src/Row.cpp
    inc/wing/Types.h
    inc/wing/Util.h src/Util.cpp
    inc/wing/Value.h src/Value.cpp
    inc/wing/WingMySQL.h src/WingMySQL.cpp
)

add_library(${PROJECT_NAME} STATIC ${LIB_WING_MYSQL_SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE /usr/include/mysql)

target_link_libraries(${PROJECT_NAME} PRIVATE
    uv
    ${MYSQL_CLIENT}
    pthread
    m
    rt
    dl
)

include(examples/CMakeLists.txt)