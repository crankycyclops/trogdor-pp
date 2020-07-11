# Standalone Client

A demo CLI client written to demonstrate how one can use the Trogdor++ library to build a single-player game.

## Dependencies

* [G++](https://gcc.gnu.org/projects/cxx-status.html) 7+ or [Clang++](https://clang.llvm.org/cxx_status.html) 8+. A different compiler might work, but it hasn't been tested, so you're on your own.
* [CMake](https://cmake.org/) 3.10 or above
* The core library, installed as a shared object or DLL in a place where CMake can find it

## Installation

To build and install the single player demo, use these commands:

```
cmake -DCMAKE_BUILD_TYPE=Release .
make standalone && make install
```

Note that you don't have to explicitly build the library first. Building the client will automatically build and link to the library as a dependency.

The client will be installed to `/usr/bin/standalone` and will, by default, use the sample game data intalled to `/usr/share/trogdor` by the core library target.

If you need debug symbols, add the following option to your cmake command: `-DCMAKE_BUILD_TYPE=Debug`. If you've already built it using debug symbols in the past and want to switch back to a release build, use this cmake option instead: `-DCMAKE_BUILD_TYPE=Release`.

If you need to install relative to a custom path, add this to your cmake command: `-DCMAKE_INSTALL_PREFIX:PATH=/your/custom/prefix`.