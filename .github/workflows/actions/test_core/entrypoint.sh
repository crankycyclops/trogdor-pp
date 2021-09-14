#!/bin/sh

cd src/core
LUA_VERSION=5.1 cmake -DCMAKE_BUILD_TYPE=Debug .

# Make sure the build for core succeeds
make -j2 trogdor

# Build and run the unit tests for core against Lua 5.1
make -j2 test_core
./test_core

make clean

LUA_VERSION=5.2 cmake -DCMAKE_BUILD_TYPE=Debug .

# Make sure the build for core succeeds
make -j2 trogdor

# Build and run the unit tests for core against Lua 5.2
make -j2 test_core
./test_core

make clean

LUA_VERSION=5.3 cmake -DCMAKE_BUILD_TYPE=Debug .

# Make sure the build for core succeeds
make -j2 trogdor

# Build and run the unit tests for core against Lua 5.3
make -j2 test_core
./test_core
