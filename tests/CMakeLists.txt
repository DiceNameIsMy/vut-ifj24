
add_executable(test_lexer test_lexer.c
        ../include/structs/stack.h
        ../include/parser/parser.h
        ../src/parser/parser.c
        ../src/structs/sym_table.c
        ../include/structs/sym_table.h)
target_link_libraries(test_lexer PRIVATE LexerLib)
target_compile_definitions(test_lexer PRIVATE DEBUG=1)

add_executable(test_token test_token.c
        ../include/structs/stack.h
        ../include/parser/parser.h
        ../src/parser/parser.c
        ../src/structs/sym_table.c
        ../include/structs/sym_table.h)
target_link_libraries(test_token PRIVATE LexerLib)
target_compile_definitions(test_token PRIVATE DEBUG=1)

add_executable(test_parser test_parser.c
        ../include/structs/stack.h
        ../src/structs/stack.c
        ../include/parser/parser.h
        ../src/parser/parser.c
        ../src/structs/sym_table.c
        ../include/structs/sym_table.h)
target_link_libraries(test_parser PRIVATE LexerLib)
target_compile_definitions(test_parser PRIVATE DEBUG=1)
