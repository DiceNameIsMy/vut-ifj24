
add_executable(test_token test_token.c test_utils.h)
target_link_libraries(test_token PRIVATE LexerLib StructsLib)
target_compile_definitions(test_token PRIVATE DEBUG=1)

add_executable(test_lexer test_lexer.c test_utils.h)
target_link_libraries(test_lexer PRIVATE LexerLib StructsLib)
target_compile_definitions(test_lexer PRIVATE DEBUG=1)

add_executable(test_dynBuffer test_dynBuffer.c test_utils.h)
target_link_libraries(test_dynBuffer PRIVATE StructsLib)
target_compile_definitions(test_dynBuffer PRIVATE DEBUG=1)
