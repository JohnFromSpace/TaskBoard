cmake_minimum_required(VERSION 3.16)

add_library(taskboard_core
    src/http_responses.cpp
)

target_include_directories(taskboard_core
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(taskboard_core PUBLIC cxx_std_17)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(taskboard_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(taskboard_backend
    src/main.cpp
)

target_link_libraries(taskboard_backend PRIVATE taskboard_core)

if (WIN32)
    target_link_libraries(taskboard_backend PRIVATE ws2_32)
endif()

add_executable(taskboard_tests
    tests/http_responses_tests.cpp
)

target_link_libraries(taskboard_tests PRIVATE taskboard_core)

add_test(NAME taskboard_tests COMMAND taskboard_tests)

