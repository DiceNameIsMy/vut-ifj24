import pytest

from main import run_compiler, run_interpreter

def test_run_compiler_and_interpreter():
    input_file_path = "./tests/input/hello_world.ifj24.zig"
    output_file_path = "./tests/output/hello_world.ifj24.zig.out"
    expected_interpeter_output = "Hello, World!\n"

    # Run the compiler
    compiler_stdout, compiler_stderr, compiler_returncode = run_compiler(input_file_path)
    assert compiler_returncode == 0, f"Compiler error: {compiler_stderr}"

    # Write the compiler output to a temporary file
    with open(output_file_path, 'w') as f:
        f.write(compiler_stdout)

    # Run the interpreter
    interpreter_stdout, interpreter_stderr, interpreter_returncode = run_interpreter(output_file_path)

    assert interpreter_returncode == 0, f"Interpreter error: {interpreter_stderr}"
    assert interpreter_stdout.strip() == expected_interpeter_output
