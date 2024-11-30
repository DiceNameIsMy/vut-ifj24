import sys
import subprocess

def get_file_heading(file_path):
    try:
        # Run the bash script
        result = subprocess.run(['bash', './scripts/get_authors_str.sh', file_path], capture_output=True, text=True)
        stdout = result.stdout if result.stdout is not None else ""
        stdout = (stdout
                  .replace("- malbert", "- Albert Popov")
                  .replace("- GladiatorEntered", "- Kraso... Ole...")
                  .replace("- Fundgod", "- Ivan Savin"))
        
        if result.stderr:
            print("Errors:" + result.stderr)
            raise Exception("An error occurred while running the script")

        return stdout
    except Exception as e:
        print(f"An error occurred while running the script: {e}")


def process_files():
    for file_path in sys.stdin:
        file_path = file_path.strip()  # Remove any leading/trailing whitespace
        if not file_path:
            continue

        try:
            heading = get_file_heading(file_path)
            new_file_contents = None

            comment_type = None
            comment_end_idx = 0
            # Open the file for reading
            with open(file_path, 'r') as file:
                for line in file:

                    # Find if first thing in a file is a commment
                    if line.startswith("/*"):
                        comment_type = "/*"
                        comment_end_idx += len(line)
                        continue
                    elif line.startswith("//"):
                        comment_type = "//"
                        comment_end_idx += len(line)
                        continue
                    
                    # If inside a comment, find the index when comment ends
                    if comment_type == "/*":
                        if "*/" in line:
                            comment_end_idx += line.index("*/") + 2
                            break
                        
                        comment_end_idx += len(line)
                        continue
                    elif comment_type == "//":
                        if not line.strip().startswith("//"):
                            print(f"Found a non-comment line at {line}")
                            break

                        comment_end_idx += len(line)
                        continue

                    # If found something other than a comment, break the loop
                    new_file_contents = heading + file.read()
                    break

            if comment_end_idx is not None:
                with open(file_path, 'r') as file:
                    new_file_contents = heading + file.read()[comment_end_idx:]

                # Write the new file contents
                with open(file_path, 'w') as file:
                    file.write(new_file_contents)

            print(f"Processed file: {file_path}")

        except Exception as e:
            print(f"An error occurred while processing '{file_path}': {e}")

if __name__ == "__main__":
    """
    Read file names from stdin and replace/append first comment with information about its.
    Can be used with `find` to append authors to every .c and .h file:
        find . -type f \( -name "*.c" -o -name "*.h" \) | python scripts/add_authors.py
    """
    process_files()