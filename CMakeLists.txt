cmake_minimum_required(VERSION 3.10.0)

project(fractles C CXX)


add_executable(
    ConsoleCraft
    ConsoleCraft.cpp
)

target_include_directories(
    ConsoleCraft
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

if(UNIX)
    target_compile_options(ConsoleCraft PRIVATE -Wall -Wextra -Wpedantic)
endif()

