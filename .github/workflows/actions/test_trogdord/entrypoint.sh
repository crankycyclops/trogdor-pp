#!/bin/sh

cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_REDIS=ON .

# Make sure the entire build for trogdord succeeds, including main.cpp, which
# isn't included in the unit tests
make -j2 trogdord

# Make sure we can generate core dumps
ulimit -c unlimited
sysctl -w kernel.core_pattern=core

# Compile and run the unit tests
make -j2 test_trogdord
timeout -s SIGQUIT 2 ./test_trogdord