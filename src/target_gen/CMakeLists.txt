project(TargetGenLib)

set(SOURCE_FILES
    instructions.c
    target_gen.c
    target_func_scope.c
    target_label_context.c
)

add_library(${PROJECT_NAME} ${SOURCE_FILES})
add_library(${PROJECT_NAME}Dbg ${SOURCE_FILES})
target_compile_definitions(${PROJECT_NAME}Dbg PRIVATE DEBUG)
