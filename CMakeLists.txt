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
    src/protocol_formatter.cpp
    src/logger.cpp
    src/resp_parser.cpp
    src/persistance.cpp
    src/config.cpp
)

target_include_directories(cpp_redis PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(cpp_redis PRIVATE ws2_32)

add_executable(redis_tests
    tests/main.cpp
    tests/database_tests.cpp
    tests/parser_tests.cpp
    tests/handler_tests.cpp
    tests/formatter_tests.cpp
    tests/tests_utils.cpp

    src/database.cpp
    src/command_parser.cpp
    src/command_handler.cpp
    src/protocol_formatter.cpp
    src/persistance.cpp
    src/config.cpp
)

target_include_directories(redis_tests PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)