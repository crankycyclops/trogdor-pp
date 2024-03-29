#include <doctest.h>

#include <trogdor/game.h>
#include <trogdor/filesystem.h>
#include <trogdor/lua/luastate.h>

#include <trogdor/entities/resource.h>
#include <trogdor/entities/creature.h>
#include <trogdor/entities/object.h>
#include <trogdor/entities/room.h>
#include <trogdor/entities/player.h>

#include <trogdor/iostream/nullout.h>
#include <trogdor/iostream/nullerr.h>

#include "luafuncs.h"
#include "testluastate.h"


// Runs basic sanity checks on new instances of LuaState
const char *SANITY_CHECK = "sanitycheck.lua";

// Tests luaL_checkudata_ex()
const char *LUAL_CHECKUDATA_EX_TEST = "test_lual_checkudata_ex.lua";

// Tests to confirm that an empty array was pushed successfully
const char *CHECK_EMPTY_ARRAY = "checkemptyarray.lua";

// Tests to confirm that a numeric array was pushed successfully
const char *CHECK_NUMERIC_ARRAY = "checknumericarray.lua";

// Tests to confirm that a mixed type array was pushed successfully
const char *CHECK_MIXED_ARRAY = "checkmixedarray.lua";


TEST_SUITE("LuaState (luastate.cpp)") {

	TEST_CASE("LuaState (luastate.cpp): Default Constructor") {

		std::cout << "Lua Version: " << trogdor::LuaState::getLuaVersion() << std::endl;

		#ifndef CORE_UNIT_TEST_LUA_ROOT
			FAIL("CORE_UNIT_TEST_LUA_ROOT must be defined.");
		#endif

		std::unique_ptr<trogdor::Game> game = std::make_unique<trogdor::Game>(
			std::make_unique<trogdor::NullErr>()
		);

		trogdor::LuaState L(game.get());

		// Load into the Lua state a simple script that will be used as a
		// sanity check that returns 1 if anything is wrong and 0 if not.
		L.loadScriptFromFile(
			std::string(CORE_UNIT_TEST_LUA_ROOT) +
			STD_FILESYSTEM::path::preferred_separator +
			SANITY_CHECK
		);

		CHECK(game.get() == L.getGame());

		// Because I'm only executing Lua states in a single thread and because
		// locking would make things more complicated, I'm not going to call
		// L.lock().
		L.call("sanityCheck");
		L.execute(1);

		if (!L.getBoolean(0)) {
			FAIL(L.getLastErrorMsg());
		}
	}

	TEST_CASE("LuaState (luastate.cpp): Copy Constructor") {

		#ifndef CORE_UNIT_TEST_LUA_ROOT
			FAIL("CORE_UNIT_TEST_LUA_ROOT must be defined.");
		#endif

		std::unique_ptr<trogdor::Game> game = std::make_unique<trogdor::Game>(
			std::make_unique<trogdor::NullErr>()
		);

		trogdor::LuaState L(game.get());

		// Load into the Lua state a simple script that will be used as a
		// sanity check that returns 1 if anything is wrong and 0 if not.
		L.loadScriptFromFile(
			std::string(CORE_UNIT_TEST_LUA_ROOT) +
			STD_FILESYSTEM::path::preferred_separator +
			SANITY_CHECK
		);

		// Make a copy of the LuaState instance and confirm that the sanityCheck
		// function is carried over and that it passes.
		trogdor::LuaState LCopy(L);

		CHECK(game.get() == LCopy.getGame());

		LCopy.call("sanityCheck");
		LCopy.execute(1);

		if (!LCopy.getBoolean(0)) {
			FAIL(L.getLastErrorMsg());
		}
	}

	TEST_CASE("LuaState (luastate.cpp): Deserialization Constructor and serialize()") {

		#ifndef CORE_UNIT_TEST_LUA_ROOT
			FAIL("CORE_UNIT_TEST_LUA_ROOT must be defined.");
		#endif

		std::unique_ptr<trogdor::Game> game = std::make_unique<trogdor::Game>(
			std::make_unique<trogdor::NullErr>()
		);

		trogdor::LuaState L(game.get());

		// Load into the Lua state a simple script that will be used as a
		// sanity check that returns 1 if anything is wrong and 0 if not.
		L.loadScriptFromFile(
			std::string(CORE_UNIT_TEST_LUA_ROOT) +
			STD_FILESYSTEM::path::preferred_separator +
			SANITY_CHECK
		);

		auto serialized = L.serialize();
		trogdor::LuaState deserializedL(game.get(), *serialized);

		CHECK(game.get() == deserializedL.getGame());

		// Like the copy constructor, the deserialized LuaState instance should
		// have all of the same scripts loaded.
		deserializedL.call("sanityCheck");
		deserializedL.execute(1);

		if (!deserializedL.getBoolean(0)) {
			FAIL(deserializedL.getLastErrorMsg());
		}
	}

	TEST_CASE("LuaState (luastate.cpp): Assignment Operator") {

		#ifndef CORE_UNIT_TEST_LUA_ROOT
			FAIL("CORE_UNIT_TEST_LUA_ROOT must be defined.");
		#endif

		std::unique_ptr<trogdor::Game> game = std::make_unique<trogdor::Game>(
			std::make_unique<trogdor::NullErr>()
		);

		trogdor::LuaState L(game.get());

		// Load into the Lua state a simple script that will be used as a
		// sanity check that returns 1 if anything is wrong and 0 if not.
		L.loadScriptFromFile(
			std::string(CORE_UNIT_TEST_LUA_ROOT) +
			STD_FILESYSTEM::path::preferred_separator +
			SANITY_CHECK
		);

		trogdor::LuaState LCopy(game.get());
		LCopy = L;

		CHECK(game.get() == LCopy.getGame());

		LCopy.call("sanityCheck");
		LCopy.execute(1);

		if (!LCopy.getBoolean(0)) {
			FAIL(L.getLastErrorMsg());
		}
	}

	TEST_CASE("LuaState (luastate.cpp): getGame()") {

		#ifndef CORE_UNIT_TEST_LUA_ROOT
			FAIL("CORE_UNIT_TEST_LUA_ROOT must be defined.");
		#endif

		std::unique_ptr<trogdor::Game> game = std::make_unique<trogdor::Game>(
			std::make_unique<trogdor::NullErr>()
		);

		trogdor::LuaState L(game.get());

		CHECK(game.get() == L.getGame());
	}

	// Warning: the way I setup these two tests is *really* janky...
	TEST_CASE("LuaState (luastate.cpp): luaL_register_wrapper()") {

		SUBCASE("Functions are not attached to a library") {

			std::unique_ptr<trogdor::Game> game = std::make_unique<trogdor::Game>(
				std::make_unique<trogdor::NullErr>()
			);

			TestLuaState L(game.get());

			static const luaL_Reg luaFunctions[] = {
				{"returnTrue", luaReturnTrue},
				{0, 0}
			};

			// From https://pgl.yoyo.org/luai/i/luaL_register, we can see that
			// we need to attach the functions to a table. All of this is stuff
			// that would ordinarily be internal to trogdor::LuaState but is
			// necessary to test this method.
			luaL_newmetatable(L.getRealState(), "TestTable");

			// TestTable.__index = TestTable
			lua_pushvalue(L.getRealState(), -1);
			lua_setfield(L.getRealState(), -2, "__index");

			L.luaL_register_wrapper(L, nullptr, luaFunctions);

			// This sets TestTable as a global so I can access it
			lua_setglobal(L.getRealState(), "TestTable");

			// Verify that returnTrue() was registered
			L.loadScriptFromString("function callReturnTrue()\nreturn TestTable.returnTrue()\nend");
			L.call("callReturnTrue");
			L.execute(1);

			if (!L.getBoolean(0)) {
				FAIL(L.getLastErrorMsg());
			}
		}

		SUBCASE("Functions are attached to a library") {

			std::unique_ptr<trogdor::Game> game = std::make_unique<trogdor::Game>(
				std::make_unique<trogdor::NullErr>()
			);

			TestLuaState L(game.get());

			static const luaL_Reg luaFunctions[] = {
				{"returnTrue", luaReturnTrue},
				{0, 0}
			};

			// I don't have to do any of what I did in the test above because
			// passing in a string for the second parameter creates the global
			// table (library) for me.
			L.luaL_register_wrapper(L, "TestTable", luaFunctions);

			// Verify that returnTrue() is registered
			L.loadScriptFromString("function callReturnTrue()\nreturn TestTable.returnTrue()\nend");
			L.call("callReturnTrue");
			L.execute(1);

			if (!L.getBoolean(0)) {
				FAIL(L.getLastErrorMsg());
			}
		}
	}

	TEST_CASE("LuaState (luastate.cpp): luaL_checkudata_ex()") {

		#ifndef CORE_UNIT_TEST_LUA_ROOT
			FAIL("CORE_UNIT_TEST_LUA_ROOT must be defined.");
		#endif

		std::unique_ptr<trogdor::Game> game = std::make_unique<trogdor::Game>(
			std::make_unique<trogdor::NullErr>()
		);

		TestLuaState L(game.get());

		static const luaL_Reg luaFunctions[] = {
			{"luaTestCheckudataex", luaTestCheckudataex},
			{0, 0}
		};

		L.loadScriptFromFile(
			std::string(CORE_UNIT_TEST_LUA_ROOT) +
			STD_FILESYSTEM::path::preferred_separator +
			LUAL_CHECKUDATA_EX_TEST
		);

		L.luaL_register_wrapper(L, "Test", luaFunctions);

		L.call("testLualCheckudataex");
		L.execute(1);

		if (!L.getBoolean(0)) {
			FAIL(L.getLastErrorMsg());
		}
	}

	TEST_CASE("LuaState (luastate.cpp): pushEntity()") {

		std::unique_ptr<trogdor::Game> game = std::make_unique<trogdor::Game>(
			std::make_unique<trogdor::NullErr>()
		);

		TestLuaState L(game.get());

		// Register this function as a global so we can call it
		lua_pushcfunction(L.getRealState(), luaTestCheckudataex);
		lua_setglobal(L.getRealState(), "luaTestCheckudataex");

		std::unique_ptr<trogdor::entity::Resource> resource = std::make_unique<trogdor::entity::Resource>(
			game.get(),
			"testresource"
		);

		std::unique_ptr<trogdor::entity::Creature> creature = std::make_unique<trogdor::entity::Creature>(
			game.get(),
			"testcreature",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		std::unique_ptr<trogdor::entity::Room> room = std::make_unique<trogdor::entity::Room>(
			game.get(),
			"testroom",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		std::unique_ptr<trogdor::entity::Object> object = std::make_unique<trogdor::entity::Object>(
			game.get(),
			"testobject",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		std::unique_ptr<trogdor::entity::Player> player = std::make_unique<trogdor::entity::Player>(
			game.get(),
			"testplayer",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		// Make sure that if we push an entity as an argument, it becomes
		// available to the called function (if the unit tests for
		// LuaState::luaL_checkudata_ex() fail, then this will quite possibly
		// fail as well.)
		L.call("luaTestCheckudataex");

		// Since I'm not really supposed to call pushEntity directly (but I
		// have to test it since it's public), I need to manually increment
		// the number of arguments (LuaState::nArgs++)
		L.pushEntity(L.getRealState(), resource.get());
		L.incNargs();
		L.pushArgument("resource");

		L.execute(1);

		if (!L.getBoolean(0)) {
			FAIL("Failed to retrieve Resource as an argument (make sure LuaState::luaL_checkudata_ex() tests passed before debugging this test)");
		}

		L.call("luaTestCheckudataex");

		L.pushEntity(L.getRealState(), creature.get());
		L.incNargs();
		L.pushArgument("creature");

		L.execute(1);

		if (!L.getBoolean(0)) {
			FAIL("Failed to retrieve Creature as an argument (make sure LuaState::luaL_checkudata_ex() tests passed before debugging this test)");
		}

		L.call("luaTestCheckudataex");

		L.pushEntity(L.getRealState(), room.get());
		L.incNargs();
		L.pushArgument("room");

		L.execute(1);

		if (!L.getBoolean(0)) {
			FAIL("Failed to retrieve Room as an argument (make sure LuaState::luaL_checkudata_ex() tests passed before debugging this test)");
		}

		L.call("luaTestCheckudataex");

		L.pushEntity(L.getRealState(), object.get());
		L.incNargs();
		L.pushArgument("object");

		L.execute(1);

		if (!L.getBoolean(0)) {
			FAIL("Failed to retrieve Object as an argument (make sure LuaState::luaL_checkudata_ex() tests passed before debugging this test)");
		}

		L.call("luaTestCheckudataex");

		L.pushEntity(L.getRealState(), player.get());
		L.incNargs();
		L.pushArgument("player");

		L.execute(1);

		if (!L.getBoolean(0)) {
			FAIL("Failed to retrieve Player as an argument (make sure LuaState::luaL_checkudata_ex() tests passed before debugging this test)");
		}
	}

	TEST_CASE("LuaState (luastate.cpp): pushArray()") {

		SUBCASE("Empty array") {

			std::unique_ptr<trogdor::Game> game = std::make_unique<trogdor::Game>(
				std::make_unique<trogdor::NullErr>()
			);

			TestLuaState L(game.get());

			L.loadScriptFromFile(
				std::string(CORE_UNIT_TEST_LUA_ROOT) +
				STD_FILESYSTEM::path::preferred_separator +
				CHECK_EMPTY_ARRAY
			);

			trogdor::LuaArray arr;
			CHECK(0 == arr.size());

			L.call("checkEmptyArray");

			// I'm not supposed to call pushArray() directly outside the Lua
			// API classes, so I have to manually increment the argument count
			L.pushArray(L.getRealState(), arr);
			L.incNargs();

			L.execute(1);

			if (!L.getBoolean(0)) {
				FAIL("Checks failed for pushing an empty array argument");
			}
		}

		SUBCASE("Array of ints") {

			std::unique_ptr<trogdor::Game> game = std::make_unique<trogdor::Game>(
				std::make_unique<trogdor::NullErr>()
			);

			TestLuaState L(game.get());

			L.loadScriptFromFile(
				std::string(CORE_UNIT_TEST_LUA_ROOT) +
				STD_FILESYSTEM::path::preferred_separator +
				CHECK_NUMERIC_ARRAY
			);

			trogdor::LuaArray arr;

			arr.push_back({trogdor::LUA_TYPE_NUMBER, 1.0});
			arr.push_back({trogdor::LUA_TYPE_NUMBER, 2.0});
			arr.push_back({trogdor::LUA_TYPE_NUMBER, 3.0});
			arr.push_back({trogdor::LUA_TYPE_NUMBER, 4.0});
			arr.push_back({trogdor::LUA_TYPE_NUMBER, 5.0});

			CHECK(5 == arr.size());

			L.call("checkNumericArray");

			L.pushArray(L.getRealState(), arr);
			L.incNargs();

			L.execute(1);

			if (!L.getBoolean(0)) {
				FAIL("Checks failed for pushing an array of numbers");
			}
		}

		SUBCASE("Array of mixed types") {

			std::unique_ptr<trogdor::Game> game = std::make_unique<trogdor::Game>(
				std::make_unique<trogdor::NullErr>()
			);

			TestLuaState L(game.get());

			L.loadScriptFromFile(
				std::string(CORE_UNIT_TEST_LUA_ROOT) +
				STD_FILESYSTEM::path::preferred_separator +
				CHECK_MIXED_ARRAY
			);

			trogdor::LuaArray nestedarr;

			nestedarr.push_back({trogdor::LUA_TYPE_NUMBER, 2.0});

			trogdor::LuaTable nestedtable;

			nestedtable.setField("string", "string");
			nestedtable.setField("number", 1.0);

			trogdor::LuaArray arr;

			arr.push_back({trogdor::LUA_TYPE_NUMBER, 1.0});
			arr.push_back({trogdor::LUA_TYPE_BOOLEAN, true});
			arr.push_back({trogdor::LUA_TYPE_STRING, std::string("wee!")});
			arr.push_back({trogdor::LUA_TYPE_ARRAY, &nestedarr});
			arr.push_back({trogdor::LUA_TYPE_TABLE, &nestedtable});

			CHECK(5 == arr.size());

			L.call("checkMixedArray");

			L.pushArray(L.getRealState(), arr);
			L.incNargs();

			L.execute(1);

			if (!L.getBoolean(0)) {
				FAIL("Checks failed for pushing an array of mixed types");
			}

		}
	}

	TEST_CASE("LuaState (luastate.cpp): pushTable()") {

		// TODO
	}

	TEST_CASE("LuaState (luastate.cpp): getLastErrorMsg()") {

		// TODO
	}

	TEST_CASE("LuaState (luastate.cpp): loadScriptFromFile()") {

		// TODO: test both valid and invalid files (as well as non-existent files)
	}

	TEST_CASE("LuaState (luastate.cpp): loadScriptFromString()") {

		// TODO: test blank string, string with syntax errors, and string with valid Lua
	}

	TEST_CASE("LuaState (luastate.cpp): All variants of pushArgument()") {

		// TODO
	}

	TEST_CASE("LuaState (luastate.cpp): pushNilArgument()") {

		// TODO
	}

	TEST_CASE("LuaState (luastate.cpp): getBoolean()") {

		// TODO
	}

	TEST_CASE("LuaState (luastate.cpp): getNumber()") {

		// TODO
	}

	TEST_CASE("LuaState (luastate.cpp): getString()") {

		// TODO
	}

	TEST_CASE("LuaState (luastate.cpp): prime()") {

		// TODO
	}

	TEST_CASE("LuaState (luastate.cpp): call() and execute()") {

		// TODO: test both defined and undefined functions and with and without arguments and return values
	}
}
