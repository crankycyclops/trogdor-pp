set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")

cmake_minimum_required(VERSION 3.14)
enable_language(C CXX)

include(FindPkgConfig)
include(CheckIncludeFile)
include(CheckIncludeFileCXX)

project(standalone VERSION 0.10.0)

# Enable conditional compilation in the case of a debug build
string(TOLOWER "${CMAKE_BUILD_TYPE}" build_type_lower)
if(build_type_lower STREQUAL "debug")
	add_definitions(-DDEBUG)
endif(build_type_lower STREQUAL "debug")

set(CMAKE_CXX_FLAGS "-Wall -pthread")
set(CMAKE_CXX_FLAGS_RELEASE -O2)

# Build using C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Don't force every optional component to build as a prerequisite to "make install"
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY True)

# We're not going to link against it, but we do need the Lua version the core
# library was compiled against to be in our include path in order to include
# its headers. By default, this was Lua version 5.3, but it's possible a
# different version was used, in which case you'll have to pass it into your
# cmake command.
if(NOT DEFINED ENV{LUA_VERSION})
	set(ENV{LUA_VERSION} 5.3)
endif(NOT DEFINED ENV{LUA_VERSION})

# Require the core library
pkg_search_module(standalone REQUIRED trogdor)

# These two are only required for their include files
pkg_search_module(LIBXML REQUIRED libxml-2.0 libxml2 libxml>=2)
pkg_search_module(LUA REQUIRED lua-$ENV{LUA_VERSION})

# See if any of the optional serialization drivers are installed
find_library(ENABLE_SERIALIZE_JSON NAMES libtrogdor_serial_json.so trogdor_serial_json)
find_library(ENABLE_SERIALIZE_SQLITE NAMES libtrogdor_serial_sqlite.so trogdor_serial_sqlite)

# Game definition files are saved here by default
add_definitions(-DGAME_XML_DEFAULT_PATH=\"${CMAKE_INSTALL_PREFIX}/share/trogdor/game.xml\")

###############################################################################

set(STANDALONE_SOURCES
	main.cpp
	streamout.cpp
	streamerr.cpp
	actions/timeaction.cpp
	actions/quitaction.cpp
)

# If any of the serialization drivers are installed, enable the load and save commands
if (ENABLE_SERIALIZE_JSON OR ENABLE_SERIALIZE_SQLITE)

	set(STANDALONE_SOURCES
		${STANDALONE_SOURCES}
		actions/saveaction.cpp
		actions/loadaction.cpp
	)

endif (ENABLE_SERIALIZE_JSON OR ENABLE_SERIALIZE_SQLITE)

# Build the example standalone game, which illustrates what a single player
# client might look like
add_executable(standalone ${STANDALONE_SOURCES})

target_link_libraries(standalone
	PUBLIC trogdor
)

# TODO: I need to refactor core's code so that I don't need these in the
# include path (they should just be required for core)
target_include_directories(standalone
	PUBLIC ${LIBXML_INCLUDE_DIRS}
	PUBLIC ${LUA_INCLUDE_DIRS}
)

if (ENABLE_SERIALIZE_JSON OR ENABLE_SERIALIZE_SQLITE)

	target_link_libraries(standalone
		PUBLIC trogdor_serial
	)

endif (ENABLE_SERIALIZE_JSON OR ENABLE_SERIALIZE_SQLITE)

if (ENABLE_SERIALIZE_JSON)

	add_definitions(-DENABLE_SERIALIZE_JSON)

	target_link_libraries(standalone
		PUBLIC trogdor_serial_json
	)

endif (ENABLE_SERIALIZE_JSON)

if (ENABLE_SERIALIZE_SQLITE)

	add_definitions(-DENABLE_SERIALIZE_SQLITE)

	target_link_libraries(standalone
		PUBLIC trogdor_serial_sqlite
	)

endif (ENABLE_SERIALIZE_SQLITE)

install(TARGETS standalone DESTINATION bin COMPONENT standalone)

###############################################################################

# Uninstall target
if(NOT TARGET uninstall)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY)

    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()

###############################################################################

# Run cppcheck (if available)
find_program(CPPCHECK cppcheck)

if (CPPCHECK)

	add_custom_target(cppcheck
		cppcheck -D GAME_XML_DEFAULT_PATH --enable=all ${PROJECT_SOURCE_DIR}/* > /dev/null
	)

else (CPPCHECK)

	add_custom_target(cppcheck echo "cppcheck not installed or not found. Skipping.")

endif (CPPCHECK)

# Perform static analysis
add_custom_target(analyze)
add_dependencies(analyze cppcheck)

###############################################################################

add_custom_target(lcount
	wc -l ${PROJECT_SOURCE_DIR}/*/*/*.cpp ${PROJECT_SOURCE_DIR}/*/*/*/*.cpp ${PROJECT_SOURCE_DIR}/*/*/*/*/*.cpp ${PROJECT_SOURCE_DIR}/*/*/*/*/*/*.cpp ${PROJECT_SOURCE_DIR}/*/*/*/*.h ${PROJECT_SOURCE_DIR}/*/*/*/*/*.h ${PROJECT_SOURCE_DIR}/*/*/*/*/*/*.h ${PROJECT_SOURCE_DIR}/*/*/*/*/*/*/*.h
)