#!/bin/sh

cd src/core
LUA_VERSION=5.1 cmake -DCMAKE_BUILD_TYPE=Debug .

if [ 0 -ne $? ]; then
    exit 1
fi

# Make sure the build for core succeeds
make -j2 trogdor

if [ 0 -ne $? ]; then
    exit 1
fi

# Build and run the unit tests for core against Lua 5.1
make -j2 test_core

if [ 0 -ne $? ]; then
    exit 1
fi

./test_core

if [ 0 -ne $? ]; then
    exit 1
fi

make clean
rm -rf CMakeFiles && rm CMakeCache.txt

#############################################

LUA_VERSION=5.2 cmake -DCMAKE_BUILD_TYPE=Debug .

if [ 0 -ne $? ]; then
    exit 1
fi

# Make sure the build for core succeeds
make -j2 trogdor

if [ 0 -ne $? ]; then
    exit 1
fi

# Build and run the unit tests for core against Lua 5.2
make -j2 test_core

if [ 0 -ne $? ]; then
    exit 1
fi

./test_core

if [ 0 -ne $? ]; then
    exit 1
fi

make clean
rm -rf CMakeFiles && rm CMakeCache.txt

#############################################

LUA_VERSION=5.3 cmake -DCMAKE_BUILD_TYPE=Debug .

if [ 0 -ne $? ]; then
    exit 1
fi

# Make sure the build for core succeeds
make -j2 trogdor

if [ 0 -ne $? ]; then
    exit 1
fi

# Build and run the unit tests for core against Lua 5.3
make -j2 test_core

if [ 0 -ne $? ]; then
    exit 1
fi

./test_core

if [ 0 -ne $? ]; then
    exit 1
fi
