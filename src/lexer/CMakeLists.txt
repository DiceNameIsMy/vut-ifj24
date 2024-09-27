project(LexerLib)

set(SOURCE_FILES
        lexer.c
)

add_library(${PROJECT_NAME} ${SOURCE_FILES})

# Specify where to find the header files
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
