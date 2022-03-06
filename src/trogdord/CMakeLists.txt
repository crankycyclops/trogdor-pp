set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")

cmake_minimum_required(VERSION 3.14)
enable_language(C CXX)

include(FindPkgConfig)
include(CheckIncludeFile)
include(CheckIncludeFileCXX)
include(LibtoolFile)

project(trogdord VERSION 0.80.0)

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

# I'm trying to cut my reliance on the Boost libraries. This ensures that we'll
# build using the standalone header-only ASIO library.
add_definitions(-DASIO_STANDALONE)

# Game definition files are saved here by default
set(GAME_XML_DEFAULT_PATH "${CMAKE_INSTALL_PREFIX}/share/trogdor/game.xml")
add_definitions(-DGAME_XML_DEFAULT_PATH=\"${GAME_XML_DEFAULT_PATH}\")

# TODO: I need to refactor core's code so that I don't need these in the
# include path (they should just be required for core)
set(CORE_INCLUDES
	PUBLIC ${LIBXML_INCLUDE_DIRS}
	PUBLIC ${LUA_INCLUDE_DIRS}
)

if (ENABLE_SERIALIZE_JSON)
	add_definitions(-DENABLE_SERIALIZE_JSON)
endif (ENABLE_SERIALIZE_JSON)

if (ENABLE_SERIALIZE_SQLITE)
	add_definitions(-DENABLE_SERIALIZE_SQLITE)
endif (ENABLE_SERIALIZE_SQLITE)

###############################################################################

# C++ Include files necessary to develop for trogdord extensions
add_custom_target(trogdord_dev)

install(DIRECTORY include/trogdord DESTINATION include COMPONENT trogdord_dev)

###############################################################################

# pkg_config for trogdord.so
add_custom_target(trogdord_pkgconfig)

set(PKGCONFIG_TARGET trogdord)
set(PKGCONFIG_NAME PROJECT_NAME)
set(PKGCONFIG_DESCRIPTION "Trogdord server")
set(PKGCONFIG_VERSION PROJECT_VERSION)
set(PKGCONFIG_URL "https://github.com/crankycyclops/trogdor-pp")

configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/pkg_config.pc.in"
	trogdord.pc
	@ONLY
)

install(FILES ${CMAKE_BINARY_DIR}/trogdord.pc DESTINATION lib/pkgconfig COMPONENT trogdor_pkgconfig)

###############################################################################

# Trogdord ini starting point

add_custom_target(trogdord_data)

install(FILES etc/trogdord.ini DESTINATION etc/trogdord COMPONENT trogdord_data)
install(DIRECTORY DESTINATION var/log COMPONENT trogdord_data)

###############################################################################

# User can either manually specify the location of the inih library or let CMake
# invoke pkg-config to look for it (this will fail on Ubuntu 18.04 and below,
# and likely others. The version of inih packaged for Ubuntu 20.04 and above
# works with pkg-config, so in that case you shouldn't have an issue.)
if(DEFINED ENV{INIH_INCLUDE_DIRS})
	message("Manually declared existence and include path of inih.")
	set(INIH_INCLUDEDIR $ENV{INIH_INCLUDE_DIRS})
else(DEFINED ENV{INIH_INCLUDE_DIRS})
	pkg_search_module(INIH REQUIRED inih)
endif(DEFINED ENV{INIH_INCLUDE_DIRS})

CHECK_INCLUDE_FILE_CXX("asio.hpp" HAVE_ASIO)

if (HAVE_ASIO)

	set(TROGDORD_SOURCES
		config.cpp
		filesystem.cpp
		dispatcher.cpp
		gamewrapper.cpp
		gamecontainer.cpp
		extensionloader.cpp
		exception/requestexception.cpp
		filter/filter.cpp
		network/tcpconnection.cpp
		network/tcpserver.cpp
		scopes/controller.cpp
		scopes/global.cpp
		scopes/game.cpp
		scopes/entity.cpp
		scopes/resource.cpp
		scopes/tangible.cpp
		scopes/place.cpp
		scopes/room.cpp
		scopes/thing.cpp
		scopes/object.cpp
		scopes/being.cpp
		scopes/creature.cpp
		scopes/player.cpp
		serial/drivermap.cpp
		io/input/listeners/listener.cpp
		io/input/listenercontainer.cpp
		io/output/driver.cpp
		io/output/local.cpp
		io/iostream/serverout.cpp
		io/iostream/servererr.cpp
	)

	set(TROGDORD_INCLUDES
		PUBLIC ${PROJECT_SOURCE_DIR}/src/core/include
		PUBLIC ${PROJECT_SOURCE_DIR}/include
		PUBLIC ${INIH_INCLUDEDIR}
	)

	set(TROGDORD_LIBRARIES_MINUS_CORE
		PUBLIC stdc++fs
		PUBLIC inih
	)

	# For now, extensions are only supported if libdl is used to load
	# shared objects. If not (like, say, you're building on Windows),
	# the extension loader will be non-functional.
	find_library(DLPATH NAMES libdl.so dl)
	if (DLPATH)
		add_definitions(-DLIBDL)
		set(TROGDORD_LIBRARIES_MINUS_CORE
			${TROGDORD_LIBRARIES_MINUS_CORE}
			PUBLIC dl
		)
	endif (DLPATH)

	if (ENABLE_REDIS)

		pkg_check_modules(HIREDIS REQUIRED hiredis>=0.13)

		set(TROGDORD_SOURCES
			${TROGDORD_SOURCES}
			io/output/redis.cpp
			io/input/listeners/redis.cpp
		)

		add_definitions(-DENABLE_REDIS)

	endif (ENABLE_REDIS)

	add_library(libtrogdord SHARED
		${TROGDORD_SOURCES}
	)

	CREATE_LIBTOOL_FILE(libtrogdord ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR} trogdord)
	set_target_properties(libtrogdord PROPERTIES OUTPUT_NAME "trogdord")

	target_include_directories(libtrogdord
		PUBLIC ${CORE_INCLUDES}
		PUBLIC ${TROGDORD_INCLUDES}
	)

	target_link_libraries(libtrogdord
		${TROGDORD_LIBRARIES_MINUS_CORE}
		PUBLIC trogdor
	)

	if (ENABLE_REDIS)

		target_include_directories(libtrogdord
			PUBLIC ${HIREDIS_INCLUDE_DIRS}
		)

		target_link_libraries(libtrogdord
			PUBLIC ${HIREDIS_LIBRARIES}
		)

	endif (ENABLE_REDIS)

	add_executable(trogdord
		main.cpp
	)

	target_link_libraries(trogdord PUBLIC libtrogdord)

	add_dependencies(libtrogdord trogdord_pkgconfig)
	add_dependencies(trogdord trogdord_data libtrogdord)

	# Explose the installation prefix to trogdord
	add_definitions(-DTROGDORD_INSTALL_PREFIX=\"${CMAKE_INSTALL_PREFIX}\")

	# Default location where trogdord looks for trogdord.ini
	add_definitions(-DTROGDORD_INI_PATH=\"${CMAKE_INSTALL_PREFIX}/etc/trogdord/trogdord.ini\")

	install(TARGETS libtrogdord DESTINATION lib COMPONENT libtrogdord OPTIONAL)
	install(TARGETS trogdord DESTINATION bin COMPONENT trogdord OPTIONAL)
	install(DIRECTORY DESTINATION var/trogdord/state COMPONENT trogdord OPTIONAL)

	###############################################################################

	# Simulates loading an extension that exports no useful symbols
	add_library(test_trogdord_nousefulsymbols SHARED test/extensions/nousefulsymbols.cpp)
	set_property(TARGET test_trogdord_nousefulsymbols PROPERTY POSITION_INDEPENDENT_CODE ON)
	set_target_properties(test_trogdord_nousefulsymbols PROPERTIES PREFIX "")

	###############################################################################

	# Simulates loading an extension that exports a duplicate scope
	add_library(test_trogdord_duplicatescope SHARED test/extensions/duplicatescope.cpp)
	set_property(TARGET test_trogdord_duplicatescope PROPERTY POSITION_INDEPENDENT_CODE ON)
	set_target_properties(test_trogdord_duplicatescope PROPERTIES PREFIX "")

	target_link_libraries(test_trogdord_duplicatescope
		PUBLIC libtrogdord
	)

	add_dependencies(test_trogdord_duplicatescope
		libtrogdord
	)

	###############################################################################

	# Simulates loading an extension that exports a duplicate output driver
	add_library(test_trogdord_duplicateoutputdriver SHARED test/extensions/duplicateoutputdriver.cpp)
	set_property(TARGET test_trogdord_duplicateoutputdriver PROPERTY POSITION_INDEPENDENT_CODE ON)
	set_target_properties(test_trogdord_duplicateoutputdriver PROPERTIES PREFIX "")

	if (ENABLE_REDIS)

		target_include_directories(test_trogdord_duplicateoutputdriver
			PUBLIC ${HIREDIS_INCLUDE_DIRS}
		)

		target_link_libraries(test_trogdord_duplicateoutputdriver
			PUBLIC ${HIREDIS_LIBRARIES}
		)

	endif (ENABLE_REDIS)

	target_link_libraries(test_trogdord_duplicateoutputdriver
		PUBLIC libtrogdord
	)

	add_dependencies(test_trogdord_duplicateoutputdriver
		libtrogdord
	)

	###############################################################################

	# Simulates loading an extension that exports a valid output driver
	add_library(test_trogdord_newoutputdriver SHARED test/extensions/newoutputdriver.cpp)
	set_property(TARGET test_trogdord_newoutputdriver PROPERTY POSITION_INDEPENDENT_CODE ON)
	set_target_properties(test_trogdord_newoutputdriver PROPERTIES PREFIX "")

	if (ENABLE_REDIS)

		target_include_directories(test_trogdord_newoutputdriver
			PUBLIC ${HIREDIS_INCLUDE_DIRS}
		)

		target_link_libraries(test_trogdord_newoutputdriver
			PUBLIC ${HIREDIS_LIBRARIES}
		)

	endif (ENABLE_REDIS)

	target_link_libraries(test_trogdord_newoutputdriver
		PUBLIC libtrogdord
	)

	add_dependencies(test_trogdord_newoutputdriver
		libtrogdord
	)

	###############################################################################

	# Simulates loading an extension that exports a valid scope
	add_library(test_trogdord_hello SHARED test/extensions/helloscope.cpp)
	set_property(TARGET test_trogdord_hello PROPERTY POSITION_INDEPENDENT_CODE ON)
	set_target_properties(test_trogdord_hello PROPERTIES PREFIX "")

	if (ENABLE_REDIS)

		target_include_directories(test_trogdord_hello
			PUBLIC ${HIREDIS_INCLUDE_DIRS}
		)

		target_link_libraries(test_trogdord_hello
			PUBLIC ${HIREDIS_LIBRARIES}
		)

	endif (ENABLE_REDIS)

	target_link_libraries(test_trogdord_hello
		PUBLIC libtrogdord
	)

	add_dependencies(test_trogdord_hello
		libtrogdord
	)

	###############################################################################

	# This is the game definition file we're going to use for unit tests
	add_definitions(
		-DCORE_UNIT_TEST_DEFINITION_FILE=\(\"${GAME_XML_DEFAULT_PATH}\"\)
	)

	# Trogdord unit tests
	add_executable(test_trogdord
		${TROGDORD_SOURCES}
		test/mock/mockscope.cpp
		test/mock/mockoutputdriver.cpp
		test/mock/mockdispatcher.cpp
		test/mock/game/mockaction.cpp
		test/main.cpp
		test/config.cpp
		test/dispatcher.cpp
		test/gamewrapper.cpp
		test/gamecontainer.cpp
		test/extensionloader.cpp
		test/io/output/driver.cpp
		test/io/output/local.cpp
		test/scopes/controller.cpp
		test/scopes/global.cpp
		test/scopes/game.cpp
		test/scopes/entity.cpp
		test/scopes/tangible.cpp
		test/scopes/place.cpp
		test/scopes/room.cpp
		test/scopes/thing.cpp
		test/scopes/object.cpp
		test/scopes/being.cpp
		test/scopes/creature.cpp
		test/scopes/player.cpp
		test/io/output/driver.cpp
		test/io/output/local.cpp
	)

	target_include_directories(test_trogdord
		PUBLIC ${CORE_INCLUDES}
		PUBLIC ${TROGDORD_INCLUDES}
		PUBLIC ${PROJECT_SOURCE_DIR}/include
	)

	target_link_libraries(test_trogdord
		${TROGDORD_LIBRARIES_MINUS_CORE}
		PUBLIC trogdor
	)

	# Separate executable that tests trogdord's use of environment variables
	add_executable(test_trogdord_envvar
		${TROGDORD_SOURCES}
		test/main.cpp
		test/envvar/config.cpp
	)

	target_include_directories(test_trogdord_envvar
		PUBLIC ${CORE_INCLUDES}
		PUBLIC ${TROGDORD_INCLUDES}
		PUBLIC ${PROJECT_SOURCE_DIR}/include
	)

	target_link_libraries(test_trogdord_envvar
		${TROGDORD_LIBRARIES_MINUS_CORE}
		PUBLIC trogdor
	)

	if (ENABLE_REDIS)

		target_include_directories(test_trogdord
			PUBLIC ${HIREDIS_INCLUDE_DIRS}
		)

		target_link_libraries(test_trogdord
			PUBLIC ${HIREDIS_LIBRARIES}
		)

		target_include_directories(test_trogdord_envvar
			PUBLIC ${HIREDIS_INCLUDE_DIRS}
		)

		target_link_libraries(test_trogdord_envvar
			PUBLIC ${HIREDIS_LIBRARIES}
		)

	endif (ENABLE_REDIS)

	# The default interval between timer ticks should be much shorter for unit tests
	target_compile_definitions(test_trogdord PRIVATE TIMER_CUSTOM_INTERVAL=5)

	add_custom_target(unit_test)

	add_dependencies(test_trogdord
		test_trogdord_nousefulsymbols
		test_trogdord_duplicatescope
		test_trogdord_duplicateoutputdriver
		test_trogdord_newoutputdriver
		test_trogdord_hello
	)

	add_dependencies(unit_test test_trogdord test_trogdord_envvar)

	###############################################################################

	if (ENABLE_SERIALIZE_JSON)

		target_link_libraries(trogdord
			PUBLIC trogdor_serial
			PUBLIC trogdor_serial_json
		)

		target_link_libraries(test_trogdord
			PUBLIC trogdor_serial
			PUBLIC trogdor_serial_json
		)

		target_link_libraries(test_trogdord_envvar
			PUBLIC trogdor_serial
			PUBLIC trogdor_serial_json
		)

	endif (ENABLE_SERIALIZE_JSON)

	if (ENABLE_SERIALIZE_SQLITE)

		target_link_libraries(trogdord
			PUBLIC trogdor_serial
			PUBLIC trogdor_serial_sqlite
		)

		target_link_libraries(test_trogdord
			PUBLIC trogdor_serial
			PUBLIC trogdor_serial_sqlite
		)

		target_link_libraries(test_trogdord_envvar
			PUBLIC trogdor_serial
			PUBLIC trogdor_serial_sqlite
		)

	endif (ENABLE_SERIALIZE_SQLITE)

else (HAVE_ASIO)

	# So that we don't cache the result and then never find it even when
	# it's later installed
	unset(HAVE_ASIO)

	# If I use literal spaces, somehow cmake ends up converting this to a string that includes backslashes
	add_custom_target(libtrogdord echo "\"Skipping\\040trogdord\\040because\\040the\\040ASIO\\040headers\\040are\\040not\\040installed\\040in\\040a\\040place\\040where\\040CMake\\040can\\040find\\040them.\"")
	add_custom_target(trogdord echo "\"Skipping\\040trogdord\\040because\\040the\\040ASIO\\040headers\\040are\\040not\\040installed\\040in\\040a\\040place\\040where\\040CMake\\040can\\040find\\040them.\"")

endif (HAVE_ASIO)

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