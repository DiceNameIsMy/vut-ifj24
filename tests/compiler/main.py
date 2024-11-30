import subprocess

"""
To run tests, install `pytest` & execute `pytest` in the terminal
"""

def run_compiler(input_file_path):
    try:
        # Open the input file
        with open(input_file_path, 'rb') as input_file:
            # Run the binary file and pass the input file contents to its stdin
            process = subprocess.Popen("./build/src/vut_ifj_debug", stdin=input_file, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

            # Capture the output and errors
            stdout, stderr = process.communicate()

            # Return the output, errors, and exit code
            return stdout.decode('utf-8'), stderr.decode('utf-8'), process.returncode

    except FileNotFoundError as ex:
        return None, f"Error: {ex}", -1
    except Exception as e:
        return None, f"An error occurred: {e}", -1


def run_interpreter(input_file_path, stdin=""):
    try:
        # Open the input file
        # Run the binary file and pass the input file contents to its stdin
        process = subprocess.Popen(["./tests/ic24int", input_file_path, "-s"], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        process.stdin.write(stdin.encode('utf-8'))
        
        # Capture the output and errors
        stdout, stderr = process.communicate()
        process.stdin.close()

        # Return the output and errors
        return stdout.decode('utf-8'), stderr.decode('utf-8'), process.returncode

    except Exception as e:
        return None, f"An error occurred: {e}", -1

