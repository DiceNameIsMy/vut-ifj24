#!/bin/bash

# Check if venv directory exists
if [ ! -d "venv" ]; then

    # Create a virtual environment
    if ! $"python -m venv venv"; then
        echo "Failed to create virtual environment"
        exit 1
    fi
fi

# Activate the virtual environment
source venv/bin/activate
if [ $? -ne 0 ]; then
    echo "Failed to activate virtual environment"
    exit 1
fi

# Run pytest
pytest
if [ $? -ne 0 ]; then
    echo "Compiler test have failed"
    exit 1
fi
