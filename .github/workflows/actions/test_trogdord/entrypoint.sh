#!/bin/bash

# Install core library as prerequisite
cd src/core
cmake -DENABLE_SERIALIZE_JSON=ON -DENABLE_SERIALIZE_SQLITE=ON -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX:PATH=/usr .
make -j2 trogdor
make install

cd ../trogdord
cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_REDIS=ON -DCMAKE_INSTALL_PREFIX:PATH=/usr .

# Make sure the entire build for trogdord succeeds, including main.cpp, which
# isn't included in the unit tests
make -j2 trogdord

# Compile and run the unit tests
make -j2 test_trogdord
timeout -s SIGQUIT 10 sudo -u app ./test_trogdord
