#!/bin/sh

cd src/core
cmake -DCMAKE_BUILD_TYPE=Debug .

# Make sure the build for core succeeds
make -j2 trogdor

# Build and run the unit tests for core
make -j2 test_core
./test_core
