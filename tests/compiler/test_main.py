import pytest

from main import run_compiler, run_interpreter


@pytest.mark.parametrize("program_name, interpreter_input, expected_interpreter_output", [
    ("hello_world.ifj24.zig", "", "Hello World!"),
    ("factorial_rec.ifj24.zig", "5\n", "Zadejte cislo pro vypocet faktorialu: Vysledek: 120"),
    # TODO: Add implicit literal conversion
    # ("factorial_iter.ifj24.zig", "5\n", "Zadejte cislo pro vypocet faktorialu: Vysledek: 120"),
])
def test_program(program_name, interpreter_input, expected_interpreter_output):
    input_file_path = f"./tests/input/{program_name}"
    output_file_path = f"./tests/output/{program_name}.ifjcode"

    # Run the compiler
    compiler_stdout, compiler_stderr, compiler_returncode = run_compiler(input_file_path)
    assert compiler_returncode == 0, f"Compiler error: {compiler_stderr}"

    # Write the compiler output to a temporary file
    with open(output_file_path, 'w') as f:
        f.write(compiler_stdout)

    # Run the interpreter
    interpreter_stdout, interpreter_stderr, interpreter_returncode = run_interpreter(output_file_path, interpreter_input)

    assert interpreter_returncode == 0, f"Interpreter error: {interpreter_stderr}"
    assert interpreter_stdout.strip() == expected_interpreter_output
