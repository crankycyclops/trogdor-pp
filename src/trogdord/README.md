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
* [INIH (INI Not Invented Here)](https://github.com/benhoyt/inih) (libinih1 and libinih-dev packages on Ubuntu)
* [RapidJSON](https://rapidjson.org/) >= 1.1.0
* [SQLite](https://www.sqlite.org/) >= 3.0
* [Hiredis](https://github.com/redis/hiredis) 0.13+ (optional, for Redis Pub/Sub support in trogdord)
* The core library, installed as a shared object or DLL in a place where CMake can find it (corresponds to the trogdor build target)

## Installation

To build and install, first make sure the [core library](../core/README.md) was built and installed to a place where pkg-config can find it, then issue these commands:

```
cd src/trogdord && cmake -DCMAKE_BUILD_TYPE=Release .
make trogdord && make install
```

To build trogdord with redis support, add `-DENABLE_REDIS=ON` to your cmake command above.

If the [core library](../core/README.md) was compiled with either the JSON or SQLite3 serialization drivers (or both), they will be available to trogdord for dumping and restoring games.

If you need debug symbols, add the following option to your cmake command: `-DCMAKE_BUILD_TYPE=Debug`. If you've already built it using debug symbols in the past and want to switch back to a release build, use this cmake option instead: `-DCMAKE_BUILD_TYPE=Release`.

If you need to install relative to a custom path, add this to your cmake command: `-DCMAKE_INSTALL_PREFIX:PATH=/your/custom/prefix`.

If you're on Ubuntu 18.04 or below and install the inih library through apt-get, you'll find that files for pkg-config aren't included, and thus, CMake won't be able to find them. You can get around this by setting the environment variable `INIH_INCLUDE_DIRS` to the path where the inih header is installed (/usr/include on Ubuntu 18.04.) Ubuntu 20.04 and above include support for pkg-config, so in that case you should be able to build without this workaround.

For example (this works on Ubuntu 18.04):

```
INIH_INCLUDE_DIRS="/usr/include" cmake -DCMAKE_BUILD_TYPE=Release .
```

Trogdord, if compiled on a platform that supports libdl for dynamic linking, supports writing and loading extensions (support for other platforms like Windows may or may not materialize in the future.) This support is automatically built-in, but can be disabled in trogdord.ini.

See the ["Hello, World"](./sample/extensions/hello) extension for a buildable example that serves as a template that can be copied and adapted for other projects.

## Copyright and License

Trogdor++ and Trogdord are licensed under the [GPL 3.0](https://www.gnu.org/licenses/gpl-3.0.en.html) and are Copyright © 2013 - Present by James Colannino.
