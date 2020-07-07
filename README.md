![Unit Tests](https://github.com/crankycyclops/trogdor-pp/workflows/Unit%20Tests/badge.svg)

# Trogdor++

A single unified engine for building text adventures and MUDs (with batteries included.)

## Introduction

Trogdor++ provides you with all the tools you need to build a solid interactive fiction experience, from offline single-player text adventures to massive online MUDs and everything in-between.

At the heart of Trogdor++ is the core library, written in C++. This library supports all the basic features necessary to build a basic game out of the box with no programming required. For features that aren't baked into the library itself, Trogdor++ also includes bindings that make it straightforward to extend using both Lua and C++.

Trogdor++ is a work in progress and more features are being added every day.

## Overview

This repository includes the following modules:

* [src/core](https://github.com/crankycyclops/trogdor-pp/tree/master/src/core): The core library, written in C++
* [src/standalone](https://github.com/crankycyclops/trogdor-pp/tree/master/src/standalone): A barebones single-player CLI client, written primarily as a demonstration for how to use the library
* [src/trogdord](https://github.com/crankycyclops/trogdor-pp/tree/master/src/trogdord): A queryable TCP server capable of hosting multi-player games with optional support for [Redis](https://redis.io/) Pub/Sub
* [src/php7/trogdord](https://github.com/crankycyclops/trogdor-pp/tree/master/src/php7/trogdord): A PHP 7 client for trogdord, implemented as an extension in C++
* [src/nodejs/trogdord](https://github.com/crankycyclops/trogdor-pp/tree/master/src/nodejs/trogdord): A Node.js client for trogdord, written in Javascript

## Dependencies

### For the Core Library

* [G++](https://gcc.gnu.org/projects/cxx-status.html) 7+ or [Clang++](https://clang.llvm.org/cxx_status.html) 8+. A different compiler might work, but it hasn't been tested, so you're on your own.
* [CMake](https://cmake.org/) 3.10 or above
* [LibXML2](http://xmlsoft.org/) (libxml2-dev package on Ubuntu)
* [Lua](https://www.lua.org/) 5.1, 5.2, or 5.3 (lua5.x-dev package on Ubuntu, where 5.x is the version you're compiling against)

### For the Standalone Client and Trogdord

* [G++](https://gcc.gnu.org/projects/cxx-status.html) 7+ or [Clang++](https://clang.llvm.org/cxx_status.html) 8+. A different compiler might work, but it hasn't been tested, so you're on your own.
* [CMake](https://cmake.org/) 3.10 or above
* The core library, installed as a shared object or DLL in a place where CMake can find it

### For the PHP 7 Extension

* [G++](https://gcc.gnu.org/projects/cxx-status.html) 7+ or [Clang++](https://clang.llvm.org/cxx_status.html) 8+. A different compiler might work, but it hasn't been tested, so you're on your own.
* [Boost ASIO](https://www.boost.org/doc/libs/1_73_0/doc/html/boost_asio.html) >=1.60
* [RapidJSON](https://rapidjson.org/) >= 1.1.0
* [PHP](https://www.php.net/) 7.2, 7.3, or 7.4, along with the include files and access to phpize

More details [here](src/core/php7/trogdord/README.md).

### For the Node.js Module

* [Node.js](https://nodejs.org/) 12+

More details [here](src/core/nodejs/trogdord/README.md).

## Installation

If you're building the game server, trogodrd, you'll also need the Boost libraries installed to a place where CMake can find it, and if you're building trogdord with redis support (required if you want to do anything with web sockets), you'll have to install hiredis 0.13 or later.

By default, trogdor-pp compiles against Lua 5.3, but will also build against 5.1 and 5.2. To set the lua version to any other version, just prefix your make command with "LUA_VERSION=5.x".

Build has been tested against Lua 5.1, 5.2, and 5.3.

To install all dependencies in one fell swoop (on Ubuntu 12.04 and up):

```
$ apt-get install cmake g++ libxml2-dev lua5.x-dev (where 5.x is your preferred Lua version)
```

Trogdor-pp itself is a library, but in src/standalone, you'll find a simple single player client that demonstrates how it can be used.

To build just the library, issue these commands:

```
cmake -DCMAKE_BUILD_TYPE=Release .
make trogdor && make install
```

To build the single player client demo, use these commands:

```
cmake -DCMAKE_BUILD_TYPE=Release .
make standalone && make install
```

To build the game server, trogdord:

```
cmake -DCMAKE_BUILD_TYPE=Release .
make trogdord && make install
```

To build the PHP 7 module that can connect to and interact with trogdord:

```
cd src/php7/trogdord
phpize && ./configure && make && make install
```

Note that the PHP module requires the rapidjson library (apt-get install rapidjson-dev on Ubuntu)

To build trogdord with redis support, add `-DENABLE_REDIS=ON` to your cmake command above.

You don't have to explicitly build the library first. Building the client will build and link to it automatically as a dependency.

To build any of the above targets with GDB debug symbols, add the following option to your cmake command: `-DCMAKE_BUILD_TYPE=Debug`

And if you've already built it using debug symbols and want to switch back to a release build without them, use this cmake option: `-DCMAKE_BUILD_TYPE=Release`

If you want to install to somewhere other than the default system path, specify the following option to your cmake command: `-DCMAKE_INSTALL_PREFIX:PATH=/custom/install/path`

## History

Trogdor++ is the C++ version of the [Trogdor Text Adventure Engine](https://github.com/crankycyclops/trogdor "Trogdor Text Adventure Engine"), originally written in C.  That project is no longer maintained but still exists on GitHub (in an archived state) for historical purposes.

Have fun! :)
