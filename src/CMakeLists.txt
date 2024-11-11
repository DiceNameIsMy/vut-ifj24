add_subdirectory(lexer)

# Configure the main executable
add_executable(vut_ifj main.c
        ../include/structs/stack.h
        structs/stack.c
        ../include/parser/parser.h
        parser/parser.c
        ../include/structs/dynBuffer.h
        structs/dynBuffer.c
        ../include/structs/ast.h
)
target_link_libraries(vut_ifj PRIVATE LexerLib)

# Configure an executable for debug compilation

add_executable(vut_ifj_debug main.c
        ../include/structs/stack.h
        ../include/parser/parser.h
        parser/parser.c
        ../include/structs/ast.h
)
target_link_libraries(vut_ifj_debug PRIVATE LexerLib)

add_executable(vut_ifj_debug main.c)
target_link_libraries(vut_ifj_debug PRIVATE LexerLibDbg StructsLibDbg)
target_compile_definitions(vut_ifj_debug PRIVATE DEBUG=1)
