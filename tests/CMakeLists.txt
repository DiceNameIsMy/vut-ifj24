
add_executable(test_lexer test_lexer.c)
target_link_libraries(test_lexer PRIVATE LexerLib)
target_compile_definitions(test_lexer PRIVATE DEBUG=1)

add_custom_target(run_test_lexer
        COMMAND test_lexer   # This runs the executable
        DEPENDS test_lexer   # This ensures my_executable is built before running
        WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}  # Optional: set the working directory
        COMMENT "Running tests for lexer after build"
)

add_dependencies(run_test_lexer test_lexer)
