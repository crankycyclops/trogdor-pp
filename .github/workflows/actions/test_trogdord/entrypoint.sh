#!/bin/sh

cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_REDIS=ON .

# Make sure the entire build for trogdord succeeds, including main.cpp, which
# isn't included in the unit tests
make -j2 trogdord

# Compile and run the unit tests
make -j2 test_trogdord
timeout 1 ./test_trogdord