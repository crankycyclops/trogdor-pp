![Unit Tests](https://github.com/crankycyclops/trogdor-pp/workflows/Unit%20Tests/badge.svg)

# Trogdor++

A single unified engine for building text adventures and MUDs (with batteries included.)

## Introduction

Trogdor++ provides you with all the tools you need to build a solid interactive fiction experience, from offline single-player text adventures to massive online MUDs and everything in-between.

At the heart of Trogdor++ is the core library, written in C++. This library supports all the basic features necessary to build a basic game out of the box with no programming required. For features that aren't baked into the library itself, Trogdor++ also includes bindings that make it straightforward to extend using both Lua and C++.

Trogdor++ is a work in progress and more features are being added all the time.

## Overview

This repository includes the following modules:

* [src/core](src/core/README.md): The core library, written in C++
* [src/standalone](src/standalone/README.md): A barebones single-player CLI client, written primarily as a demonstration for how to use the library
* [src/trogdord](src/trogdord/README.md): A queryable TCP server capable of hosting multi-player games with optional support for [Redis](https://redis.io/) Pub/Sub
* [src/php7/trogdord](src/php7/trogdord/README.md): A PHP 7 client for trogdord, implemented as an extension in C++
* [src/nodejs/trogdord](src/nodejs/trogdord/README.md): A Node.js client for trogdord, written in Javascript

Descriptions of each component, along with build instructions, can be found by clicking on the links above.

## History

Trogdor++ is the C++ version of the [Trogdor Text Adventure Engine](https://github.com/crankycyclops/trogdor "Trogdor Text Adventure Engine"), originally written in C.  That project is no longer maintained but still exists on GitHub (in an archived state) for historical purposes.

The name Trogdor itself comes from [this video](https://www.youtube.com/watch?v=90X5NJleYJQ) ;)

Have fun!