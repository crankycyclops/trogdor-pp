#!/bin/sh

# Build and run the unit tests for core
# TODO: right now, I'm just testing that trogdord builds. In the future, when I
# actually have unit tests to build, I'll have to replace this make target with
# test_trogdord and run the result like so: ./test_trogdord
cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_REDIS=ON .
make -j2 trogdord
