#!/bin/bash

cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_REDIS=ON .

# Make sure the entire build for trogdord succeeds, including main.cpp, which
# isn't included in the unit tests
make -j2 trogdord

# Make sure we can generate core dumps
ulimit -c unlimited
systemctl enable apport.service start
cat /proc/sys/kernel/core_pattern

# Compile and run the unit tests
make -j2 test_trogdord
timeout -s SIGQUIT 5 ./test_trogdord