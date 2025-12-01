cmake_minimum_required(VERSION 3.16)

add_executable(taskboard_backend
    src/main.cpp
)

target_compile_features(taskboard_backend PRIVATE cxx_std_17)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(taskboard_backend PRIVATE -Wall -Wextra -Wpedantic)
endif()

if (WIN32)
    target_link_libraries(taskboard_backend PRIVATE ws2_32)
endif()

