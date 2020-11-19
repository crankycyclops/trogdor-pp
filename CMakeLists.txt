cmake_minimum_required(VERSION 3.10)

include(FindPkgConfig)
include(CheckIncludeFile)
include(CheckIncludeFileCXX)

project(Trogdor-pp)

# Enable conditional compilation in the case of a debug build
string(TOLOWER "${CMAKE_BUILD_TYPE}" build_type_lower)
if(build_type_lower STREQUAL "debug")
	add_definitions(-DDEBUG)
endif(build_type_lower STREQUAL "debug")

# By default, we compile against Lua 5.3, but 5.1 and 5.2 have also been tested
# and are known to work.
if(NOT DEFINED ENV{LUA_VERSION})
	set(ENV{LUA_VERSION} 5.3)
endif()

set(CMAKE_CXX_FLAGS "-Wall -pthread")
set(CMAKE_CXX_FLAGS_RELEASE -O2)

# Build using C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Don't force every optional component to build as a prerequisite to "make install"
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY True)

# If we're building trogdord, this indicates that we should build it with redis support
option(ENABLE_REDIS "Enable Redis support" OFF)

# I'm trying to cut my reliance on the Boost libraries. This ensures that we'll
# build using the standalone header-only ASIO library.
add_definitions(-DASIO_STANDALONE)

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

# Trogdord ini starting point

add_custom_target(trogdord_data)

install(FILES src/trogdord/etc/trogdord.ini DESTINATION etc/trogdord COMPONENT trogdord_data)
install(DIRECTORY DESTINATION var/log COMPONENT trogdord_data)

###############################################################################

pkg_search_module(LIBXML REQUIRED libxml-2.0 libxml2 libxml>=2)
pkg_search_module(LUA REQUIRED lua-$ENV{LUA_VERSION})

set(CORE_SOURCES
	src/core/actions/action.cpp
	src/core/actions/cuss.cpp
	src/core/actions/inventory.cpp
	src/core/actions/look.cpp
	src/core/actions/read.cpp
	src/core/actions/take.cpp
	src/core/actions/drop.cpp
	src/core/actions/move.cpp
	src/core/actions/attack.cpp
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
	src/core/parser/parsers/inform7/lexer.cpp
	src/core/parser/parsers/inform7parser.cpp
	src/core/parser/parsers/xmlparser.cpp
	src/core/instantiator/instantiator.cpp
	src/core/instantiator/instantiators/runtime.cpp
	src/core/lua/luastate.cpp
	src/core/lua/api/luagame.cpp
	src/core/lua/api/entities/luabeing.cpp
	src/core/lua/api/entities/luacreature.cpp
	src/core/lua/api/entities/luaentity.cpp
	src/core/lua/api/entities/luaresource.cpp
	src/core/lua/api/entities/luatangible.cpp
	src/core/lua/api/entities/luaobject.cpp
	src/core/lua/api/entities/luaplace.cpp
	src/core/lua/api/entities/luaplayer.cpp
	src/core/lua/api/entities/luaroom.cpp
	src/core/lua/api/entities/luathing.cpp
	src/core/iostream/nullout.cpp
	src/core/iostream/nullerr.cpp
	src/core/iostream/placeout.cpp
	src/core/iostream/trogout.cpp
	src/core/iostream/trogerr.cpp
	src/core/event/event.cpp
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
	src/core/entities/resource.cpp
	src/core/entities/tangible.cpp
	src/core/entities/object.cpp
	src/core/entities/place.cpp
	src/core/entities/player.cpp
	src/core/entities/room.cpp
	src/core/entities/thing.cpp
	src/core/serial/driver.cpp
)

set(CORE_INCLUDES
	PUBLIC ${LIBXML_INCLUDE_DIRS}
	PUBLIC ${LUA_INCLUDE_DIRS}
	PUBLIC ${PROJECT_SOURCE_DIR}/src/core/include
)

set(CORE_LIBRARIES
	PUBLIC stdc++fs
	PUBLIC xml2
	PUBLIC lua$ENV{LUA_VERSION}
)

add_library(trogdor SHARED ${CORE_SOURCES})
target_include_directories(trogdor ${CORE_INCLUDES})
target_link_libraries(trogdor ${CORE_LIBRARIES})

# Installing the library should also install the headers and sample data
add_dependencies(trogdor dev data)

install(TARGETS trogdor DESTINATION lib COMPONENT trogdor OPTIONAL)

###############################################################################

# Build the example standalone game, which illustrates what a single player
# client might look like
add_executable(standalone
	src/standalone/main.cpp
	src/standalone/streamout.cpp
	src/standalone/streamerr.cpp
	src/standalone/actions/timeaction.cpp
	src/standalone/actions/quitaction.cpp
	src/standalone/actions/saveaction.cpp
	src/standalone/actions/loadaction.cpp
	src/standalone/serial/json.cpp
)

add_dependencies(standalone trogdor)

target_link_libraries(standalone
	PUBLIC trogdor
)

target_include_directories(standalone
	PUBLIC ${PROJECT_SOURCE_DIR}/src/core/include
)

# Default location where standalone looks for game.xml
add_definitions(-DGAME_XML_DEFAULT_PATH=\"${CMAKE_INSTALL_PREFIX}/share/trogdor/game.xml\")

install(TARGETS standalone DESTINATION bin COMPONENT standalone OPTIONAL)

###############################################################################

# User can either manually specify the location of the inih library or let CMake
# invoke pkg-config to look for it (this will fail on Ubuntu 18.04 and below,
# and likely others. The version of inih packaged for Ubuntu 20.04 and above
# works with pkg-config, so in that case you shouldn't have an issue.)
if(DEFINED ENV{INIH_INCLUDE_DIRS})
	message("Manually declared existence and include path of inih.")
	set(INIH_INCLUDEDIR $ENV{INIH_INCLUDE_DIRS})
else(DEFINED ENV{INIH_INCLUDE_DIRS})
	pkg_check_modules(INIH inih)
endif(DEFINED ENV{INIH_INCLUDE_DIRS})

if (INIH_INCLUDEDIR)

	CHECK_INCLUDE_FILE_CXX("asio.hpp" HAVE_ASIO)

	if (HAVE_ASIO)

		set(TROGDORD_SOURCES
			src/trogdord/config.cpp
			src/trogdord/filesystem.cpp
			src/trogdord/request.cpp
			src/trogdord/response.cpp
			src/trogdord/dispatcher.cpp
			src/trogdord/gamewrapper.cpp
			src/trogdord/gamecontainer.cpp
			src/trogdord/exception/requestexception.cpp
			src/trogdord/filter/filter.cpp
			src/trogdord/network/tcpconnection.cpp
			src/trogdord/network/tcpserver.cpp
			src/trogdord/scopes/controller.cpp
			src/trogdord/scopes/global.cpp
			src/trogdord/scopes/game.cpp
			src/trogdord/scopes/entity.cpp
			src/trogdord/scopes/resource.cpp
			src/trogdord/scopes/tangible.cpp
			src/trogdord/scopes/place.cpp
			src/trogdord/scopes/room.cpp
			src/trogdord/scopes/thing.cpp
			src/trogdord/scopes/object.cpp
			src/trogdord/scopes/being.cpp
			src/trogdord/scopes/creature.cpp
			src/trogdord/scopes/player.cpp
			src/trogdord/io/input/listeners/listener.cpp
			src/trogdord/io/input/listenercontainer.cpp
			src/trogdord/io/output/driver.cpp
			src/trogdord/io/output/local.cpp
			src/trogdord/io/iostream/serverout.cpp
			src/trogdord/io/iostream/servererr.cpp
		)

		set(TROGDORD_INCLUDES
			PUBLIC ${PROJECT_SOURCE_DIR}/src/core/include
			PUBLIC ${INIH_INCLUDEDIR}
		)

		set(TROGDORD_LIBRARIES_MINUS_CORE
			PUBLIC stdc++fs
			PUBLIC inih
		)

		if (ENABLE_REDIS)

			pkg_check_modules(HIREDIS REQUIRED hiredis>=0.13)

			add_library(trogdord_redis STATIC
				src/trogdord/io/output/redis.cpp
				src/trogdord/io/input/listeners/redis.cpp
			)

			target_include_directories(trogdord_redis
				PUBLIC ${PROJECT_SOURCE_DIR}/src/core/include
			)

		endif (ENABLE_REDIS)

		add_executable(trogdord
			src/trogdord/main.cpp
			${TROGDORD_SOURCES}
		)

		# Link in the redis input and output drivers
		if (ENABLE_REDIS)

			target_include_directories(trogdord_redis
				PUBLIC ${HIREDIS_INCLUDE_DIRS}
			)

			target_link_libraries(trogdord
				PRIVATE trogdord_redis
				PUBLIC ${HIREDIS_LIBRARIES}
			)

			add_definitions(-DENABLE_REDIS)
			add_dependencies(trogdord trogdord_redis)

		endif (ENABLE_REDIS)

		target_include_directories(trogdord ${TROGDORD_INCLUDES})
		target_link_libraries(trogdord ${TROGDORD_LIBRARIES_MINUS_CORE})
		target_link_libraries(trogdord PUBLIC trogdor)

		add_dependencies(trogdord trogdor trogdord_data)

		# Explose the installation prefix to trogdord
		add_definitions(-DTROGDORD_INSTALL_PREFIX=\"${CMAKE_INSTALL_PREFIX}\")

		# Default location where trogdord looks for trogdord.ini
		add_definitions(-DTROGDORD_INI_PATH=\"${CMAKE_INSTALL_PREFIX}/etc/trogdord/trogdord.ini\")

		install(TARGETS trogdord DESTINATION bin COMPONENT trogdord OPTIONAL)

	else (HAVE_ASIO)

		# So that we don't cache the result and then never find it even when
		# it's later installed
		unset(HAVE_ASIO)

		# If I use literal spaces, somehow cmake ends up converting this to a string that includes backslashes
		add_custom_target(trogdord echo "\"Skipping\\040trogdord\\040because\\040the\\040ASIO\\040headers\\040are\\040not\\040installed\\040in\\040a\\040place\\040where\\040CMake\\040can\\040find\\040them.\"")

	endif (HAVE_ASIO)

else (INIH_INCLUDEDIR)

	add_custom_target(trogdord echo "\"Skipping\\040trogdord\\040because\\040the\\040inih\\040library\\040isn't\\040installed\\040in\\040a\\040place\\040where\\040CMake\\040can\\040find\\040it\\040(you\\040can\\040try\\040setting\\040the\\040INIH_INCLUDE_DIRS\\040environment\\040variable\\040manually.)\"")

endif (INIH_INCLUDEDIR)

###############################################################################

add_custom_target(lcount
	wc -l ${PROJECT_SOURCE_DIR}/*/*/*.cpp ${PROJECT_SOURCE_DIR}/*/*/*/*.cpp ${PROJECT_SOURCE_DIR}/*/*/*/*/*.cpp ${PROJECT_SOURCE_DIR}/*/*/*/*/*/*.cpp ${PROJECT_SOURCE_DIR}/*/*/*/*.h ${PROJECT_SOURCE_DIR}/*/*/*/*/*.h ${PROJECT_SOURCE_DIR}/*/*/*/*/*/*.h ${PROJECT_SOURCE_DIR}/*/*/*/*/*/*/*.h
)

###############################################################################

# Run cppcheck (if available)
find_program(CPPCHECK cppcheck)

if (CPPCHECK)

	add_custom_target(cppcheck
		cppcheck -D GAME_XML_DEFAULT_PATH --enable=all ${PROJECT_SOURCE_DIR}/src/* > /dev/null
	)

else (CPPCHECK)

	add_custom_target(cppcheck echo "cppcheck not installed or not found. Skipping.")

endif (CPPCHECK)

# Perform static analysis
add_custom_target(analyze)
add_dependencies(analyze cppcheck)

###############################################################################

# C++ unit tests for core and trogdord (requires doctest to be installed --
# see: https://github.com/onqtam/doctest)

# This is the game definition file we're going to use for unit tests
add_definitions(
	-DCORE_UNIT_TEST_DEFINITION_FILE=\(\"${PROJECT_SOURCE_DIR}/game.xml\"\)
)

# A special statically-linked build of the core library just for unit tests
add_library(_trogdor_test STATIC ${CORE_SOURCES})
target_include_directories(_trogdor_test ${CORE_INCLUDES})
target_link_libraries(_trogdor_test ${CORE_LIBRARIES})

# The default interval between timer ticks should be much shorter for unit tests
target_compile_definitions(_trogdor_test PRIVATE TIMER_CUSTOM_INTERVAL=5)

# Core unit tests
add_executable(test_core
	src/core/test/main.cpp
	src/core/test/tokenizer.cpp
	src/core/test/messages.cpp
	src/core/test/vocabulary.cpp
	src/core/test/command.cpp
	src/core/test/utility.cpp
	src/core/test/entities/entity.cpp
	src/core/test/entities/resource.cpp
	src/core/test/entities/tangible.cpp
	src/core/test/event/eventlistener.cpp
	src/core/test/event/triggers/deathdrop.cpp
	src/core/test/event/triggers/respawn.cpp
	src/core/test/timer/timer.cpp
	src/core/test/timer/timerjob.cpp
	src/core/test/timer/jobs/autoattack.cpp
	src/core/test/timer/jobs/respawn.cpp
	src/core/test/timer/jobs/wander.cpp
	src/core/test/mock/mockentity.cpp
	src/core/test/mock/mockaction.cpp
	src/core/test/mock/mocktrigger.cpp
	src/core/test/mock/mocktimerjob.cpp
)

target_include_directories(test_core
	PUBLIC ${PROJECT_SOURCE_DIR}/src/core/include
)

target_link_libraries(test_core
	PUBLIC _trogdor_test
)

add_dependencies(test_core _trogdor_test)

if (INIH_INCLUDEDIR)

	# Trogdord unit tests
	add_executable(test_trogdord
		${TROGDORD_SOURCES}
		src/trogdord/test/mock/mockscope.cpp
		src/trogdord/test/mock/mockdispatcher.cpp
		src/trogdord/test/mock/game/mockaction.cpp
		src/trogdord/test/main.cpp
		src/trogdord/test/config.cpp
		src/trogdord/test/dispatcher.cpp
		src/trogdord/test/gamewrapper.cpp
		src/trogdord/test/gamecontainer.cpp
		src/trogdord/test/scopes/controller.cpp
		src/trogdord/test/scopes/global.cpp
		src/trogdord/test/scopes/game.cpp
		src/trogdord/test/scopes/entity.cpp
		src/trogdord/test/scopes/tangible.cpp
		src/trogdord/test/scopes/place.cpp
		src/trogdord/test/scopes/room.cpp
		src/trogdord/test/scopes/thing.cpp
		src/trogdord/test/scopes/object.cpp
		src/trogdord/test/scopes/being.cpp
		src/trogdord/test/scopes/creature.cpp
		src/trogdord/test/scopes/player.cpp
	)

	target_include_directories(test_trogdord
		PUBLIC ${CORE_INCLUDES} ${TROGDORD_INCLUDES}
		PUBLIC ${PROJECT_SOURCE_DIR}/src/trogdord/include
	)

	target_link_libraries(test_trogdord
		PUBLIC _trogdor_test
		PUBLIC ${TROGDORD_LIBRARIES_MINUS_CORE}
	)

	if (ENABLE_REDIS)

		target_include_directories(test_trogdord PUBLIC ${HIREDIS_INCLUDE_DIRS})

		target_link_libraries(test_trogdord
			PUBLIC trogdord_redis
			PUBLIC ${HIREDIS_LIBRARIES}
		)

	endif (ENABLE_REDIS)

else (INIH_INCLUDEDIR)

	add_custom_target(test_trogdord echo "\"Skipping\\040trogdord\\040unit\\040tests\\040because\\040the\\040inih\\040library\\040isn't\\040installed\\040in\\040a\\040place\\040where\\040CMake\\040can\\040find\\040it\\040(you\\040can\\040try\\040setting\\040the\\040INIH_INCLUDE_DIRS\\040environment\\040variable\\040manually.)\"")

endif (INIH_INCLUDEDIR)

add_custom_target(unit_test)
add_dependencies(unit_test test_core test_trogdord)

###############################################################################

# Uninstall target
if(NOT TARGET uninstall)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY)

    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()
