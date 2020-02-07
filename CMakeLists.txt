cmake_minimum_required(VERSION 3.10)
include(FindPkgConfig)

project(Trogdor-pp)

# By default, we compile against Lua 5.1, but 5.2 and 5.3 have also been tested
# and are known to work.
if(NOT DEFINED ENV{LUA_VERSION})
	set(ENV{LUA_VERSION} 5.1)
endif()

set(CMAKE_CXX_FLAGS "-Wall -pthread")
set(CMAKE_CXX_FLAGS_RELEASE -O2)

# Build using C++17.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Don't force every optional component to build as a prerequisite to "make install"
SET(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY True)

###############################################################################

# C++ Include files necessary to develop for the library
add_custom_target(dev)

install(DIRECTORY src/core/include/trogdor DESTINATION include COMPONENT dev)

###############################################################################

# Sample game data
add_custom_target(data)

install(FILES game.xml DESTINATION share/trogdor COMPONENT data)
install(DIRECTORY script DESTINATION share/trogdor COMPONENT data)

###############################################################################

add_library(trogdor SHARED
	src/core/action.cpp
	src/core/actions.cpp
	src/core/command.cpp
	src/core/game.cpp
	src/core/messages.cpp
	src/core/tokenizer.cpp
	src/core/utility.cpp
	src/core/vocabulary.cpp
	src/core/exception/beingexception.cpp
	src/core/exception/parseexception.cpp
	src/core/languages/english.cpp
	src/core/parser/ast.cpp
	src/core/parser/data.cpp
	src/core/parser/parser.cpp
	src/core/parser/parsers/inform7lexer.cpp
	src/core/parser/parsers/inform7parser.cpp
	src/core/parser/parsers/xmlparser.cpp
	src/core/instantiator/instantiator.cpp
	src/core/instantiator/instantiators/runtime.cpp
	src/core/lua/luastate.cpp
	src/core/lua/api/luagame.cpp
	src/core/lua/api/entities/luabeing.cpp
	src/core/lua/api/entities/luacreature.cpp
	src/core/lua/api/entities/luaentity.cpp
	src/core/lua/api/entities/luaobject.cpp
	src/core/lua/api/entities/luaplace.cpp
	src/core/lua/api/entities/luaplayer.cpp
	src/core/lua/api/entities/luaroom.cpp
	src/core/lua/api/entities/luathing.cpp
	src/core/iostream/nullin.cpp
	src/core/iostream/nullout.cpp
	src/core/iostream/nullerr.cpp
	src/core/iostream/placeout.cpp
	src/core/iostream/trogin.cpp
	src/core/iostream/trogout.cpp
	src/core/iostream/trogerr.cpp
	src/core/event/eventhandler.cpp
	src/core/event/eventlistener.cpp
	src/core/event/eventtrigger.cpp
	src/core/event/triggers/autoattack.cpp
	src/core/event/triggers/deathdrop.cpp
	src/core/event/triggers/luaeventtrigger.cpp
	src/core/event/triggers/respawn.cpp
	src/core/timer/timer.cpp
	src/core/timer/timerjob.cpp
	src/core/timer/jobs/autoattack.cpp
	src/core/timer/jobs/respawn.cpp
	src/core/timer/jobs/wander.cpp
	src/core/entities/being.cpp
	src/core/entities/creature.cpp
	src/core/entities/entity.cpp
	src/core/entities/object.cpp
	src/core/entities/place.cpp
	src/core/entities/player.cpp
	src/core/entities/room.cpp
	src/core/entities/thing.cpp
)

# Installing the library should also install the headers and sample data
add_dependencies(trogdor dev data)

pkg_search_module(LIBXML REQUIRED libxml-2.0 libxml2 libxml>=2)
pkg_search_module(LUA REQUIRED lua-$ENV{LUA_VERSION})

target_include_directories(trogdor
	PUBLIC ${LIBXML_INCLUDE_DIRS}
	PUBLIC ${LUA_INCLUDE_DIRS}
	PUBLIC ${PROJECT_SOURCE_DIR}/src/core/include
)

target_link_libraries(trogdor
	PUBLIC stdc++fs
	PUBLIC xml2
	PUBLIC lua$ENV{LUA_VERSION}
)

install(TARGETS trogdor DESTINATION lib COMPONENT trogdor OPTIONAL)

###############################################################################

# Build the example standalone game, which illustrates what a single player
# client might look like
add_executable(standalone
	src/standalone/main.cpp
	src/standalone/streamin.cpp
	src/standalone/streamout.cpp
	src/standalone/streamerr.cpp
	src/standalone/actions/timeaction.cpp
)

add_dependencies(standalone trogdor)

target_link_libraries(standalone
	PUBLIC trogdor
)

# Default location where standalone looks for game.xml
add_definitions(-DGAME_XML_DEFAULT_PATH=\"${CMAKE_INSTALL_PREFIX}/share/trogdor/game.xml\")

install(TARGETS standalone DESTINATION bin COMPONENT standalone OPTIONAL)

###############################################################################

add_custom_target(lcount
	wc -l */*/*.cpp */*/*/*.cpp */*/*/*/*.cpp */*/*/*/*/*.cpp */*/*/*.h */*/*/*/*.h */*/*/*/*/*.h */*/*/*/*/*/*.h
)

###############################################################################

# uninstall target
if(NOT TARGET uninstall)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY)

    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()
