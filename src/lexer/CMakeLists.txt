project(LexerLib)

set(SOURCE_FILES
        lexer.c
        token.c
)

add_library(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE StructsLib)

add_library(${PROJECT_NAME}Dbg ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME}Dbg PRIVATE StructsLib)

target_compile_definitions(${PROJECT_NAME}Dbg PRIVATE DEBUG)

