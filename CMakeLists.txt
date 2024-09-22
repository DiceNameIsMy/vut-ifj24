cmake_minimum_required(VERSION 3.29)
project(vut_ifj C)

set(CMAKE_C_STANDARD 17)

set(HEADER_FILES
    src/logging.h
    src/lexer/lexer.h
)
set(SOURCE_FILES
    src/lexer/lexer.c
)
add_executable(vut_ifj src/main.c ${SOURCE_FILES} ${HEADER_FILES})
add_executable(vut_ifj_debug src/main.c ${SOURCE_FILES} ${HEADER_FILES})

target_compile_definitions(vut_ifj_debug PRIVATE DEBUG=1)
