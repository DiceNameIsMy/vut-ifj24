project(StructsLib)

set(SOURCE_FILES
        ast.c
        bvs.c
        dynBuffer.c
        stack.c
        symtable.c
        queue.c
)

add_library(${PROJECT_NAME} ${SOURCE_FILES})
add_library(${PROJECT_NAME}Dbg ${SOURCE_FILES})
target_compile_definitions(${PROJECT_NAME}Dbg PRIVATE DEBUG)
