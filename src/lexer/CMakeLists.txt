project(LexerLib)

set(SOURCE_FILES
        lexer.c
        lexeme.c
)

add_library(${PROJECT_NAME} ${SOURCE_FILES})
