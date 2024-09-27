cmake_minimum_required(VERSION 3.29)

project(vut_ifj C)

set(CMAKE_C_STANDARD 17)

# Add the public include directory for all targets
include_directories(${CMAKE_SOURCE_DIR}/include)

add_subdirectory(${CMAKE_SOURCE_DIR}/src)
add_subdirectory(${CMAKE_SOURCE_DIR}/tests)
