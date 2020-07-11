#!/bin/sh

# Build and run the unit tests for core
cmake -DCMAKE_BUILD_TYPE=Debug .
make -j2 test_core
./test_core
