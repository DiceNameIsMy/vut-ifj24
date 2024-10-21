project(StructsLib)

set(SOURCE_FILES
        bvs.c
        dynBuffer.c
)

add_library(${PROJECT_NAME} ${SOURCE_FILES})
