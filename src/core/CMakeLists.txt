cmake_minimum_required(VERSION 3.14)
enable_language(C CXX)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Current version of the library
set(TROGDOR_VERSION_MAJOR 0)
set(TROGDOR_VERSION_MINOR 91)
set(TROGDOR_VERSION_PATCH 4)

configure_file(${CMAKE_CURRENT_LIST_DIR}/cmake/version.h.in ${CMAKE_CURRENT_BINARY_DIR}/version.h @ONLY)

add_definitions(
	-DTROGDOR_VERSION_MAJOR=${TROGDOR_VERSION_MAJOR}
	-DTROGDOR_VERSION_MINOR=${TROGDOR_VERSION_MINOR}
	-DTROGDOR_VERSION_PATCH=${TROGDOR_VERSION_PATCH}
)

# Determines where pkgconfig files go. By default, they go in $PREFIX/lib/pkgconfig,
# but when building binary packages (such as for Debian or Ubuntu), you might need
# to change this path to something else.
if(DEFINED ENV{PKGCONFIG_DESTINATION})
	set(PKGCONFIG_DESTINATION $ENV{PKGCONFIG_DESTINATION})
else()
    set(PKGCONFIG_DESTINATION lib/pkgconfig)
endif()

include(FindPkgConfig)
include(CheckIncludeFile)
include(CheckIncludeFileCXX)
include(LibtoolFile)

project(trogdor VERSION ${TROGDOR_VERSION_MAJOR}.${TROGDOR_VERSION_MINOR}.${TROGDOR_VERSION_PATCH})

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

# If this is turned on, we'll build the JSON serialization library
option(ENABLE_SERIALIZE_JSON "Enable JSON Serialization Format" OFF)

# If this is turned on, we'll build support the SQLite serialization ibrary
option(ENABLE_SERIALIZE_SQLITE "Enable SQLite Serialization Format" OFF)

# These two pkg-config variables will stay the same for all component libraries
set(PKGCONFIG_VERSION ${PROJECT_VERSION})
set(PKGCONFIG_URL "https://github.com/crankycyclops/trogdor-pp")

# Required libraries we have to search for before proceeding with the build
pkg_search_module(LIBXML REQUIRED libxml-2.0 libxml2 libxml>=2)
pkg_search_module(LUA REQUIRED lua-$ENV{LUA_VERSION})

# Game definition files are saved here by default
add_definitions(-DGAME_XML_DEFAULT_PATH=\"${CMAKE_INSTALL_PREFIX}/share/trogdor/game.xml\")

###############################################################################

# Auto-generated version.h
add_custom_target(trogdor_version_h)

add_custom_command(
	TARGET trogdor_version_h
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/version.h ${CMAKE_INSTALL_PREFIX}/include/trogdor/version.h
)

###############################################################################

# C++ Include files necessary to develop for the library. This is no longer
# a dependency of the trogdor target, since there are times when you would want
# to build and install the library without the headers (for example: building
# binary packages.) This will now automatically pull in the pkgconfig target.
add_custom_target(install_dev)

add_custom_command(
	TARGET install_dev
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/include/trogdor ${CMAKE_INSTALL_PREFIX}/include/trogdor
)

add_dependencies(install_dev trogdor_version_h trogdor_pkgconfig)

###############################################################################

# Sample game data
add_custom_target(data)

install(FILES data/game.xml DESTINATION share/trogdor COMPONENT data OPTIONAL)
install(DIRECTORY data/script DESTINATION share/trogdor COMPONENT data OPTIONAL)

###############################################################################

# pkg_config for trogdor.so
add_custom_target(trogdor_pkgconfig)

set(PKGCONFIG_TARGET ${PROJECT_NAME})
set(PKGCONFIG_NAME ${PROJECT_NAME})
set(PKGCONFIG_DESCRIPTION "The trogdor-pp core library")

configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/pkg_config.pc.in"
	trogdor.pc
	@ONLY
)

add_custom_command(
	TARGET trogdor_pkgconfig
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/trogdor.pc ${CMAKE_INSTALL_PREFIX}/${PKGCONFIG_DESTINATION}/trogdor.pc
)

###############################################################################

set(CORE_INCLUDES
	PUBLIC ${LIBXML_INCLUDE_DIRS}
	PUBLIC ${LUA_INCLUDE_DIRS}
	PUBLIC ${PROJECT_SOURCE_DIR}/include
)

set(CORE_LIBRARIES
	PUBLIC stdc++fs
	PUBLIC xml2
	PUBLIC lua$ENV{LUA_VERSION}
)

###############################################################################

# pkg_config for trogdor_serial.so
add_custom_target(trogdor_serial_pkgconfig)

set(PKGCONFIG_TARGET "${PROJECT_NAME}_serial")
set(PKGCONFIG_NAME "${PROJECT_NAME}_serial")
set(PKGCONFIG_DESCRIPTION "The trogdor-pp serialization driver")

configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/pkg_config.pc.in"
	trogdor_serial.pc
	@ONLY
)

add_custom_command(
	TARGET trogdor_serial_pkgconfig
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/trogdor_serial.pc ${CMAKE_INSTALL_PREFIX}/${PKGCONFIG_DESTINATION}/trogdor_serial.pc
)

set(SERIAL_SOURCES
    serial/driver.cpp
)

add_library(trogdor_serial SHARED ${SERIAL_SOURCES})
set_target_properties(trogdor_serial PROPERTIES
	VERSION ${TROGDOR_VERSION_MAJOR}.${TROGDOR_VERSION_MINOR}.${TROGDOR_VERSION_PATCH}
	SOVERSION ${TROGDOR_VERSION_MAJOR}
)
CREATE_LIBTOOL_FILE(trogdor_serial ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
target_include_directories(trogdor_serial ${CORE_INCLUDES})
add_dependencies(trogdor_pkgconfig trogdor_serial_pkgconfig)
install(TARGETS trogdor_serial LIBRARY DESTINATION lib COMPONENT trogdor_serial NAMELINK_SKIP)

###############################################################################

# Compile in optional support for JSON serialization
if (ENABLE_SERIALIZE_JSON)

    # pkg_config for trogdor_serial_json.so
    add_custom_target(trogdor_serial_json_pkgconfig)

    set(PKGCONFIG_TARGET "${PROJECT_NAME}_serial_json")
    set(PKGCONFIG_NAME "${PROJECT_NAME}_serial_json")
    set(PKGCONFIG_DESCRIPTION "The trogdor-pp JSON serialization driver")

    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/pkg_config.pc.in"
        trogdor_serial_json.pc
        @ONLY
    )

	add_custom_command(
		TARGET trogdor_serial_json_pkgconfig
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/trogdor_serial_json.pc ${CMAKE_INSTALL_PREFIX}/${PKGCONFIG_DESTINATION}/trogdor_serial_json.pc
	)

	set(JSON_SOURCES
		serial/drivers/json.cpp
	)

	add_definitions(-DENABLE_SERIALIZE_JSON)

    add_library(trogdor_serial_json SHARED ${JSON_SOURCES})
	set_target_properties(trogdor_serial_json PROPERTIES
		VERSION ${TROGDOR_VERSION_MAJOR}.${TROGDOR_VERSION_MINOR}.${TROGDOR_VERSION_PATCH}
		SOVERSION ${TROGDOR_VERSION_MAJOR}
	)
    CREATE_LIBTOOL_FILE(trogdor_serial_json ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
	target_link_libraries(trogdor_serial_json ${CORE_LIBRARIES})
	target_include_directories(trogdor_serial_json ${CORE_INCLUDES})
    add_dependencies(trogdor_serial_json trogdor_serial)
	add_dependencies(trogdor_serial_pkgconfig trogdor_serial_json_pkgconfig)
    install(TARGETS trogdor_serial_json LIBRARY DESTINATION lib COMPONENT trogdor_serial_json OPTIONAL NAMELINK_SKIP)

endif (ENABLE_SERIALIZE_JSON)

###############################################################################

# Compile in optional support for SQLite3 serialization
if (ENABLE_SERIALIZE_SQLITE)

	pkg_search_module(SQLITE REQUIRED sqlite3)

    # pkg_config for trogdor_serial_sqlite.so
    add_custom_target(trogdor_serial_sqlite_pkgconfig)

    set(PKGCONFIG_TARGET "${PROJECT_NAME}_serial_sqlite")
    set(PKGCONFIG_NAME "${PROJECT_NAME}_serial_sqlite")
    set(PKGCONFIG_DESCRIPTION "The trogdor-pp SQLite3 serialization driver")

    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/pkg_config.pc.in"
        trogdor_serial_sqlite.pc
        @ONLY
    )

	add_custom_command(
		TARGET trogdor_serial_sqlite_pkgconfig
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/trogdor_serial_sqlite.pc ${CMAKE_INSTALL_PREFIX}/${PKGCONFIG_DESTINATION}/trogdor_serial_sqlite.pc
	)

	set(SQLITE_SOURCES
		serial/drivers/sqlite.cpp
	)

	set(SQLITE_LIBRARIES
		${CORE_LIBRARIES}
		PUBLIC sqlite3
	)

	add_definitions(-DENABLE_SERIALIZE_SQLITE)

    add_library(trogdor_serial_sqlite SHARED ${SQLITE_SOURCES})
	set_target_properties(trogdor_serial_sqlite PROPERTIES
		VERSION ${TROGDOR_VERSION_MAJOR}.${TROGDOR_VERSION_MINOR}.${TROGDOR_VERSION_PATCH}
		SOVERSION ${TROGDOR_VERSION_MAJOR}
	)
    CREATE_LIBTOOL_FILE(trogdor_serial_sqlite ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
    target_include_directories(trogdor_serial_sqlite ${CORE_INCLUDES})
    target_link_libraries(trogdor_serial_sqlite ${SQLITE_LIBRARIES})
    add_dependencies(trogdor_serial_json trogdor_serial)
	add_dependencies(trogdor_serial_pkgconfig trogdor_serial_sqlite_pkgconfig)
    install(TARGETS trogdor_serial_sqlite LIBRARY DESTINATION lib COMPONENT trogdor_serial_sqlite OPTIONAL NAMELINK_SKIP)

endif (ENABLE_SERIALIZE_SQLITE)

###############################################################################

set(CORE_SOURCES
	actions/action.cpp
	actions/cuss.cpp
	actions/inventory.cpp
	actions/look.cpp
	actions/read.cpp
	actions/take.cpp
	actions/drop.cpp
	actions/move.cpp
	actions/attack.cpp
	command.cpp
	game.cpp
	messages.cpp
	tokenizer.cpp
	utility.cpp
	vocabulary.cpp
	exception/beingexception.cpp
	exception/parseexception.cpp
	languages/english.cpp
	parser/ast.cpp
	parser/data.cpp
	parser/parser.cpp
	parser/parsers/inform7/lexer.cpp
	parser/parsers/inform7parser.cpp
	parser/parsers/xmlparser.cpp
	instantiator/instantiator.cpp
	instantiator/instantiators/runtime.cpp
	lua/luastate.cpp
	lua/api/luagame.cpp
	lua/api/entities/luabeing.cpp
	lua/api/entities/luacreature.cpp
	lua/api/entities/luaentity.cpp
	lua/api/entities/luaresource.cpp
	lua/api/entities/luatangible.cpp
	lua/api/entities/luaobject.cpp
	lua/api/entities/luaplace.cpp
	lua/api/entities/luaplayer.cpp
	lua/api/entities/luaroom.cpp
	lua/api/entities/luathing.cpp
	iostream/nullout.cpp
	iostream/nullerr.cpp
	iostream/placeout.cpp
	iostream/trogout.cpp
	iostream/trogerr.cpp
	event/event.cpp
	event/eventhandler.cpp
	event/eventlistener.cpp
	event/eventtrigger.cpp
	event/triggers/autoattack.cpp
	event/triggers/deathdrop.cpp
	event/triggers/luaeventtrigger.cpp
	event/triggers/respawn.cpp
	timer/timer.cpp
	timer/timerjob.cpp
	timer/jobs/autoattack.cpp
	timer/jobs/respawn.cpp
	timer/jobs/wander.cpp
	entities/being.cpp
	entities/creature.cpp
	entities/entity.cpp
	entities/resource.cpp
	entities/tangible.cpp
	entities/object.cpp
	entities/place.cpp
	entities/player.cpp
	entities/room.cpp
	entities/thing.cpp
)

add_library(trogdor SHARED ${CORE_SOURCES})
set_target_properties(trogdor PROPERTIES
	VERSION ${TROGDOR_VERSION_MAJOR}.${TROGDOR_VERSION_MINOR}.${TROGDOR_VERSION_PATCH}
	SOVERSION ${TROGDOR_VERSION_MAJOR}
)
CREATE_LIBTOOL_FILE(trogdor ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})

target_include_directories(trogdor ${CORE_INCLUDES})
target_link_libraries(trogdor ${CORE_LIBRARIES})

add_dependencies(trogdor trogdor_serial)

if (ENABLE_SERIALIZE_JSON)
    add_dependencies(trogdor trogdor_serial_json)
endif (ENABLE_SERIALIZE_JSON)

if (ENABLE_SERIALIZE_SQLITE)
    add_dependencies(trogdor trogdor_serial_sqlite)
endif (ENABLE_SERIALIZE_SQLITE)

# Installing the library should also install the sample data
add_dependencies(trogdor data)

install(TARGETS trogdor LIBRARY DESTINATION lib COMPONENT trogdor NAMELINK_SKIP)

###############################################################################

# C++ unit tests for core and trogdord (requires doctest to be installed --
# see: https://github.com/onqtam/doctest)

# This is the game definition file we're going to use for unit tests
add_definitions(
	-DCORE_UNIT_TEST_DEFINITION_FILE=\(\"${PROJECT_SOURCE_DIR}/data/game.xml\"\)
	-DCORE_UNIT_TEST_LUA_ROOT=\(\"${PROJECT_SOURCE_DIR}/test/lua/scripts\"\)
)

set(STATIC_CORE_SOURCES
    ${CORE_SOURCES}
    ${SERIAL_SOURCES}
)

# A special statically-linked build of the core library just for unit tests
add_library(_trogdor_test STATIC ${STATIC_CORE_SOURCES})
target_include_directories(_trogdor_test ${CORE_INCLUDES})
target_link_libraries(_trogdor_test ${CORE_LIBRARIES})

# The default interval between timer ticks should be much shorter for unit tests
target_compile_definitions(_trogdor_test PRIVATE TIMER_CUSTOM_INTERVAL=5)

# Core unit tests
add_executable(test_core
	test/main.cpp
	test/tokenizer.cpp
	test/messages.cpp
	test/vocabulary.cpp
	test/command.cpp
	test/utility.cpp
	test/entities/entity.cpp
	test/entities/resource.cpp
	test/entities/tangible.cpp
	test/event/eventlistener.cpp
	test/event/triggers/deathdrop.cpp
	test/event/triggers/respawn.cpp
	test/timer/timer.cpp
	test/timer/timerjob.cpp
	test/timer/jobs/autoattack.cpp
	test/timer/jobs/respawn.cpp
	test/timer/jobs/wander.cpp
	test/mock/mockentity.cpp
	test/mock/mockaction.cpp
	test/mock/mocktrigger.cpp
	test/mock/mocktimerjob.cpp
	test/lua/luafuncs.cpp
	test/lua/luastate.cpp
)

target_include_directories(test_core
	PUBLIC ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(test_core
	PUBLIC _trogdor_test
)

add_dependencies(test_core _trogdor_test)

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

add_custom_target(lcount
	wc -l ${PROJECT_SOURCE_DIR}/*/*/*.cpp ${PROJECT_SOURCE_DIR}/*/*/*/*.cpp ${PROJECT_SOURCE_DIR}/*/*/*/*/*.cpp ${PROJECT_SOURCE_DIR}/*/*/*/*/*/*.cpp ${PROJECT_SOURCE_DIR}/*/*/*/*.h ${PROJECT_SOURCE_DIR}/*/*/*/*/*.h ${PROJECT_SOURCE_DIR}/*/*/*/*/*/*.h ${PROJECT_SOURCE_DIR}/*/*/*/*/*/*/*.h
)