#!/bin/sh

cd $GITHUB_WORKSPACE/trogdor-pp/trogdor-pp
#git checkout $GITHUB_REF_NAME

# Build and run the unit tests for core
cmake -DCMAKE_BUILD_TYPE=Debug .
make -j2 test_core
./test_core
