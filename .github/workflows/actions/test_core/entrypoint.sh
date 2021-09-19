#!/bin/sh

runTest() {

    cd src/core

    LUA_VERSION=$1 cmake -DCMAKE_BUILD_TYPE=Debug .

    if [ 0 -ne $? ]; then
        exit 1
    fi

    # Make sure the build for core succeeds
    make -j2 trogdor

    if [ 0 -ne $? ]; then
        exit 1
    fi

    # Build and run the unit tests for core against the specified Lua version
    make -j2 test_core

    if [ 0 -ne $? ]; then
        exit 1
    fi

    ./test_core

    if [ 0 -ne $? ]; then
        exit 1
    fi

    make clean

    if [ 0 -ne $? ]; then
        exit 1
    fi

    rm -rf CMakeFiles && rm CMakeCache.txt

    if [ 0 -ne $? ]; then
        exit 1
    fi
}

###############################################################################

runTest("5.1")
runTest("5.2")
runTest("5.3")
