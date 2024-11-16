#!/bin/bash

# Initialize a variable to track the exit status
exit_status=0

# Read newline-separated filenames from stdin
while IFS= read -r file; do
    # Check if the file is executable
    if [[ -x "$file" ]]; then
        # Execute the file. If failed, Capture the exit status of the command
        if ! "$file"; then
            # Set exit_status to 1 if any command fails
            exit_status=1
        fi
    else
        echo "File $file is not executable or does not exist." >&2
        # Mark as failure if the file is not executable
        exit_status=1
    fi
done

echo
echo "All tests runners have succeeded"

# Exit with the final status (0 if all succeeded, 1 if any failed)
exit $exit_status