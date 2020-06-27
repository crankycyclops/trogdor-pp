# Unit Tests for the Core Library

## Dependencies

Requires [Doctest](https://github.com/onqtam/doctest "Doctest GitHub Page").

## How to build and run

From the directory where you've configured cmake, run:

```
make test_core && ./test_core
```

To build all unit tests, not just those written for core, replace the make command above with:

```
make unit_test
```