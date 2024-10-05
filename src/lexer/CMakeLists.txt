project(LexerLib)

set(SOURCE_FILES
        lexer.c
        lexeme.c
        token.c
        ../../include/structs/stack.h
        ../../include/parser/parser.h
        ../parser/parser.c
)

add_library(${PROJECT_NAME} ${SOURCE_FILES}
        ../../include/structs/stack.h
        ../../include/parser/parser.h
        ../parser/parser.c)
