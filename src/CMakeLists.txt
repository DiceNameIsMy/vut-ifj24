add_subdirectory(lexer)
add_subdirectory(structs)

# Configure the main executable
add_executable(vut_ifj main.c)
target_link_libraries(vut_ifj PRIVATE LexerLib StructsLib)

# Configure an executable for debug compilation
add_executable(vut_ifj_debug main.c)
target_link_libraries(vut_ifj_debug PRIVATE LexerLib StructsLib)
target_compile_definitions(vut_ifj_debug PRIVATE DEBUG=1)
