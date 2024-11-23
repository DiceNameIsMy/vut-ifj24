#!/bin/bash

# Exit on errors
set -e

mkdir -p cmake-build-debug
cmake --build cmake-build-debug

find . -type f -wholename "./cmake-build-debug/tests/test_*" -executable | ./scripts/run_tests.sh