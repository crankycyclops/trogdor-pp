![Unit Tests](https://github.com/crankycyclops/trogdor-pp/workflows/Unit%20Tests/badge.svg)

# Trogdor++ Core Library

A C++ library capable of hosting multi-player text adventures and MUDs.

## Dependencies

* [G++](https://gcc.gnu.org/projects/cxx-status.html) 7+ or [Clang++](https://clang.llvm.org/cxx_status.html) 8+. A different compiler might work, but it hasn't been tested, so you're on your own.
* [CMake](https://cmake.org/) 3.10 or above
* [LibXML2](http://xmlsoft.org/) (libxml2-dev package on Ubuntu)
* [Lua](https://www.lua.org/) 5.1, 5.2, or 5.3 (lua5.x-dev package on Ubuntu, where 5.x is the version you're compiling against)

By default, trogdor-pp compiles against Lua 5.3, but will also build against 5.1 and 5.2. To set the lua version to any other version, just prefix your make command with "LUA_VERSION=5.x".

Build has been tested against Lua 5.1, 5.2, and 5.3.

To install all dependencies in one fell swoop on Ubuntu 18.04 and up:

```
$ apt-get install cmake g++ libxml2-dev lua5.x-dev (where 5.x is your preferred Lua version)
```

## Installation

To build and install the library, issue these commands:

```
cmake -DCMAKE_BUILD_TYPE=Release .
make trogdor && make install
```

The library will be installed along with the include files for development in `/usr/include/trogdor` and sample game data in `/usr/share/trogdor`.

If you need debug symbols, add the following option to your cmake command: `-DCMAKE_BUILD_TYPE=Debug`. If you've already built it using debug symbols in the past and want to switch back to a release build, use this cmake option instead: `-DCMAKE_BUILD_TYPE=Release`.

If you need to install relative to a custom path, add this to your cmake command: `-DCMAKE_INSTALL_PREFIX:PATH=/your/custom/prefix`.