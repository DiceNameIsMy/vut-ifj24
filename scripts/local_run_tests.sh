mkdir -p cmake-build-debug
cd cmake-build-debug || exit
cmake ./..
cd ./..

find . -type f -wholename "./cmake-build-debug/tests/test_*" -executable | ./scripts/run_tests.sh