
add_executable(test_lexer test_lexer.c
        ../include/structs/stack.h
        ../include/parser/parser.h
        ../src/parser/parser.c
        test_parser.c)
target_link_libraries(test_lexer PRIVATE LexerLib)
target_compile_definitions(test_lexer PRIVATE DEBUG=1)

add_executable(test_token test_token.c
        ../include/structs/stack.h
        ../include/parser/parser.h
        ../src/parser/parser.c
        test_parser.c)
target_link_libraries(test_token PRIVATE LexerLib)
target_compile_definitions(test_token PRIVATE DEBUG=1)
