cmake_minimum_required(VERSION 3.16)

project(cpp_redis VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(cpp_redis
    src/main.cpp
    src/database.cpp
    src/command_parser.cpp
    src/command_handler.cpp
    src/server.cpp
)

target_include_directories(cpp_redis PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(cpp_redis PRIVATE ws2_32)