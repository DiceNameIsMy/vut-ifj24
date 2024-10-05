
add_executable(test_lexer test_lexer.c)
target_link_libraries(test_lexer PRIVATE LexerLib)
target_compile_definitions(test_lexer PRIVATE DEBUG=1)

add_executable(test_token test_token.c)
target_link_libraries(test_token PRIVATE LexerLib)
target_compile_definitions(test_token PRIVATE DEBUG=1)
