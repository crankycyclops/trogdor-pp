# Trogdord

A TCP server built on top of the Trogdor++ library for hosting multi-player games.

## Dependencies

* [G++](https://gcc.gnu.org/projects/cxx-status.html) 7+ or [Clang++](https://clang.llvm.org/cxx_status.html) 8+. A different compiler might work, but it hasn't been tested, so you're on your own.
* [CMake](https://cmake.org/) 3.10 or above
* [Boost ASIO](https://www.boost.org/doc/libs/1_73_0/doc/html/boost_asio.html) >=1.60 (boost-system-dev package on Ubuntu)
* [Boost Ptree](https://www.boost.org/doc/libs/1_73_0/doc/html/property_tree.html) >=1.60 (boost-system-dev package on Ubuntu)
* [Hiredis](https://github.com/redis/hiredis) 0.13+ (optional, for redis Pub/Sub support in trogdord)
* The core library, installed as a shared object or DLL in a place where CMake can find it

## Installation

To build and install, issue these commands:

```
cmake -DCMAKE_BUILD_TYPE=Release .
make trogdord && make install
```

To build trogdord with redis support, add `-DENABLE_REDIS=ON` to your cmake command above.

If you need debug symbols, add the following option to your cmake command: `-DCMAKE_BUILD_TYPE=Debug`. If you've already built it using debug symbols in the past and want to switch back to a release build, use this cmake option instead: `-DCMAKE_BUILD_TYPE=Release`.

If you need to install relative to a custom path, add this to your cmake command: `-DCMAKE_INSTALL_PREFIX:PATH=/your/custom/prefix`.