#include <doctest.h>

#include <trogdor/game.h>
#include <trogdor/filesystem.h>
#include <trogdor/lua/luastate.h>
#include <trogdor/iostream/nullerr.h>


// Runs basic sanity checks on new instances of LuaState
const char *SANITY_CHECK = "sanitycheck.lua";


TEST_SUITE("LuaState (luastate.cpp)") {

	TEST_CASE("LuaState (luastate.cpp): Default Constructor") {

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

	TEST_CASE("LuaState (luastate.cpp): luaL_register_wrapper()") {

        // TODO
    }

	TEST_CASE("LuaState (luastate.cpp): luaL_checkudata_ex()") {

        // TODO
    }

	TEST_CASE("LuaState (luastate.cpp): pushEntity()") {

        // TODO
    }

	TEST_CASE("LuaState (luastate.cpp): pushArray()") {

        // TODO
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

        // TODO: test both defined and undefined functions
    }
}
