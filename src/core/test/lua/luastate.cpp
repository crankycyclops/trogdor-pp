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

        LCopy.call("sanityCheck");
        LCopy.execute(1);

        if (!LCopy.getBoolean(0)) {
            FAIL(L.getLastErrorMsg());
        }
    }

	TEST_CASE("LuaState (luastate.cpp): Deserialization Constructor") {

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

        // Like the copy constructor, the deserialized LuaState instance should
        // have all of the same scripts loaded.
        deserializedL.call("sanityCheck");
        deserializedL.execute(1);

        if (!deserializedL.getBoolean(0)) {
            FAIL(deserializedL.getLastErrorMsg());
        }
    }
}
