#!/bin/bash

cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_REDIS=ON .

# Make sure the entire build for trogdord succeeds, including main.cpp, which
# isn't included in the unit tests
make -j2 trogdord

# Make sure we can generate core dumps
ulimit -c unlimited
mkdir -p .config/apport
echo " [main] unpackaged=true" > .config/apport/settings
service apport start

# Compile and run the unit tests
make -j2 test_trogdord
timeout -s SIGQUIT 10 ./test_trogdord
ls /var/lib/systemd/coredump