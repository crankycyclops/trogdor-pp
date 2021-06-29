cmake_minimum_required(VERSION 3.10)
project("Hello World Extension")
enable_language(C CXX)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_STATIC_LIBRARY_PREFIX "")

include(FindPkgConfig)
include(CheckIncludeFile)
include(CheckIncludeFileCXX)

pkg_search_module(TROGDORD REQUIRED trogdord)

set (SOURCES
    hello.cpp
    extension.cpp
)

set(LIBRARIES
	PUBLIC trogdord
)

add_library(hello SHARED ${SOURCES})
set_target_properties(hello PROPERTIES PREFIX "")
install(TARGETS hello DESTINATION lib/trogdord)