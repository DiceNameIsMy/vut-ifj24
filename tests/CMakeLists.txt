

add_executable(test_lexer test_lexer.c test_utils.h)
target_link_libraries(test_lexer PRIVATE ParserLibDbg LexerLibDbg StructsLibDbg)
target_compile_definitions(test_lexer PRIVATE DEBUG=1)

add_executable(test_token test_token.c test_utils.h)
target_link_libraries(test_token PRIVATE ParserLibDbg LexerLibDbg StructsLibDbg)
target_compile_definitions(test_token PRIVATE DEBUG=1)

add_executable(test_parser test_parser.c)
target_link_libraries(test_parser PRIVATE ParserLibDbg LexerLibDbg StructsLibDbg)
target_compile_definitions(test_parser PRIVATE DEBUG=1)

add_executable(test_dynBuffer test_dynBuffer.c test_utils.h)
target_link_libraries(test_dynBuffer PRIVATE StructsLibDbg)

add_executable(test_bvs test_bvs.c test_utils.h)
target_link_libraries(test_bvs PRIVATE StructsLibDbg)

