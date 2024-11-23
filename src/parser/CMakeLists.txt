project(ParserLib)

set(SOURCE_FILES
        parser.c
        semantic_checks.c
)

add_library(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE StructsLib LexerLib)

add_library(${PROJECT_NAME}Dbg ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME}Dbg PRIVATE StructsLibDbg LexerLib)

target_compile_definitions(${PROJECT_NAME}Dbg PRIVATE DEBUG)

