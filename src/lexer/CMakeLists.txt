project(LexerLib)

set(SOURCE_FILES
        lexer.c
        lexeme.c
        token.c
        ../../include/structs/stack.h
        ../../include/parser/parser.h
        ../parser/parser.c
        ../structs/sym_table.c
        ../../include/structs/sym_table.h
)

add_library(${PROJECT_NAME} ${SOURCE_FILES}
        ../../include/structs/stack.h
        ../../include/parser/parser.h
        ../parser/parser.c
        ../structs/sym_table.c
        ../../include/structs/sym_table.h)
