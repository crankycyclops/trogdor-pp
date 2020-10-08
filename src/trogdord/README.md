![Trogdord Tests](https://github.com/crankycyclops/trogdor-pp/workflows/Trogdord%20Tests/badge.svg)

# Trogdord

A TCP server built on top of the Trogdor++ library for the purpose of hosting one or more multi-player games.

## Introduction

The Trogdor++ core library supports running many multi-player games in parallel, but without a driving application to take advantage of those features, the library would be worthless. Trogdord fills this gap by wrapping around core to host and manage games over a network.

Trogdord is designed to work in tandem with a client-side connector, such as the official [PHP extension](src/php7/trogdord/README.md) or the [Node.js](src/nodejs/trogdord/README.md) module, both of which are provided by this repository.

## Documentation

### For Users

The following links detail how to build, install, configure, and use trogdord.

1. [Dependencies](#dependencies)
2. [Installation](#installation)
3. [Copyright and License](#copyright-and-license)
4. [What Can Trogdord Do?](./docs/what-can-trogdord-do.md)
5. [Quickstart Guide](./docs/quickstart-guide.md)
6. [Configuration](./docs/configuration.md)
7. [Security](./docs/security.md)

### For Developers

If you're interested in writing your own client-side connector, or just want to learn more about how trogdord works, you'll find the below links beneficial.

1. [Methods, Scopes, and Actions](./docs/developer/scopes-and-actions.md)
	- [Methods](./docs/developer/scopes-and-actions.md#methods)
	- [Scopes](./docs/developer/scopes-and-actions.md#scopes)
	- [Actions](./docs/developer/scopes-and-actions.md#actions)
	- [A Complete Reference](./docs/developer/scopes-and-actions.md#a-complete-reference)
2. [Connections and Requests](./docs/developer/connections-and-requests.md)
	- [Establishing a Connection](./docs/developer/connections-and-requests.md#establishing-a-connection)
	- [Making a Request](./docs/developer/connections-and-requests.md#making-a-request)
3. [Filters](./docs/developer/filters.md)
4. [Optional Integration with Redis](./docs/developer/integration-with-redis.md)

## Dependencies

* [G++](https://gcc.gnu.org/projects/cxx-status.html) 7+ or [Clang++](https://clang.llvm.org/cxx_status.html) 8+. A different compiler might work, but it hasn't been tested, so you're on your own.
* [CMake](https://cmake.org/) 3.10 or above
* [Standalone ASIO library](https://think-async.com/Asio/) >=1.10 (libasio-dev package on Ubuntu)
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

## Copyright and License

Trogdor++ and Trogdord are licensed under the [GPL 3.0](https://www.gnu.org/licenses/gpl-3.0.en.html) and are Copyright © 2013 - Present by James Colannino.
