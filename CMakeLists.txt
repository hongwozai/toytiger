cmake_minimum_required(VERSION 3.9)

project(toytiger)

set(CMAKE_CXX_STANDARD 11)

aux_source_directory(utils SRC_UTILS)
aux_source_directory(parsing SRC_PARSING)

add_executable(toytiger ${SRC_UTILS} ${SRC_PARSING})
target_include_directories(toytiger PUBLIC ${PROJECT_SOURCE_DIR})
target_compile_options(toytiger PUBLIC -g)