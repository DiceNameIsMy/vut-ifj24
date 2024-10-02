cmake_minimum_required(VERSION 3.29)

project(vut_ifj C)

set(CMAKE_C_STANDARD 17)

# Register all header files
include_directories(${CMAKE_SOURCE_DIR}/src)

add_subdirectory(${CMAKE_SOURCE_DIR}/src)
add_subdirectory(${CMAKE_SOURCE_DIR}/tests)
