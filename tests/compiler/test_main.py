import os
import pytest

from main import run_compiler, run_interpreter


@pytest.mark.parametrize("program_name, interpreter_input, expected_interpreter_output", [
    ("hello_world.ifj24.zig", "", "Hello World!\n"),
    ("conditions.ifj24.zig", "", "\n".join([
        "if(a == 1)", 
        "if(a == 2) else", 
        "if(a == 3) else if(a == 1)", 
        "if(a == 4) else if(a == 5) else",
        "if(b)", 
        "if(1 == 0) else if(b)", 
        "if(c) else", 
        "if(1 == 0) else if(c) else", 
        "while(d < 2)", 
        "while(d < 2)", 
        ""
    ])),
    ("factorial_rec.ifj24.zig", "5\n", "Zadejte cislo pro vypocet faktorialu: Vysledek: 120"),
    ("stdfunc.ifj24.zig", "", "\n".join([
        "ifj.string: str1str2",
        "ifj.ord: 49",
        "ifj.chr: A",
        "ifj.strcmp: 0-111-1",
        "ifj.substring: strt",
        ""
    ])),
    # TODO: Add implicit literal conversion
    # ("factorial_iter.ifj24.zig", "5\n", "Zadejte cislo pro vypocet faktorialu: Vysledek: 120"),
])
def test_program(program_name, interpreter_input, expected_interpreter_output):
    input_file_path = f"./tests/input/{program_name}"
    output_file_path = f"./tests/output/{program_name}.ifjcode"

    try:
        # Run the compiler
        compiler_stdout, compiler_stderr, compiler_returncode = run_compiler(input_file_path)
        assert compiler_returncode == 0, f"Compiler error: {compiler_stderr}"

        # Write the compiler output to a temporary file
        with open(output_file_path, 'w') as f:
            f.write(compiler_stdout)

        # Run the interpreter
        interpreter_stdout, interpreter_stderr, interpreter_returncode = run_interpreter(output_file_path, interpreter_input)

        assert interpreter_returncode == 0, f"Interpreter error: {interpreter_stderr}"
        assert interpreter_stdout == expected_interpreter_output
    finally:
        # Cleanup
        if os.path.exists(output_file_path):
            os.remove(output_file_path)
            print(f"Deleted file: {output_file_path}")
        else:
            print(f"File does not exist: {output_file_path}")
        