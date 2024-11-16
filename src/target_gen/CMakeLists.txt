project(TargetGen)

set(SOURCE_FILES
    target_gen.c
    instructions.c
)

add_library(${PROJECT_NAME} ${SOURCE_FILES})
add_library(${PROJECT_NAME}Dbg ${SOURCE_FILES})
target_compile_definitions(${PROJECT_NAME}Dbg PRIVATE DEBUG)
