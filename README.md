# VUT-IFJ24

---

### Development

There are several executables specified using cmake.

- vut_ifj: Compiler without logging turned on.
- vut_ifj_debug: Has logging enabled.
- test_*: Used to run tests. More on how to use them below.

##### How to include/link .h and .c files?

Look at how lexer is done.

Create folders `src/<component_name>` and `include/<component_name>`

Create `src/<component_name>/CMakeLists.txt` & write to it something similar to `src/lexer/CMakeLists.txt`
```cmake
project(<PascalCaseComponentName>Lib)

set(SOURCE_FILES
        lexer.c
        lexeme.c
)

add_library(${PROJECT_NAME} ${SOURCE_FILES})
add_library(${PROJECT_NAME}Dbg ${SOURCE_FILES})
```

To use the component in main.c, add the following to the `src/CMakeLists.txt`
```cmake
target_link_libraries(vut_ifj PRIVATE LexerLib)
target_link_libraries(vut_ifj_debug PRIVATE LexerLibDbg)
```

##### Testing

To add your own tests, you can create a new test_<component_name>.c file with a `main` function defined. Run your tests, temporary code or whatever there. To be able to run it you'll need to add the following content to the `tests/CMakeLists.txt` file:

```cmake

add_executable(test_<component_name> test_<component_name>.c)
target_link_libraries(test_<component_name> PRIVATE <Lib1> <Lib2>) # Specify the library you are testing & its dependencies
target_compile_definitions(test_<component_name> PRIVATE DEBUG=1)

```

Replace <name> with your values. If you are using Clion you'll get your executable added right after the build.
