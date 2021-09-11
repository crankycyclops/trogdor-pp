#include <doctest.h>

#include <trogdor/game.h>
#include <trogdor/filesystem.h>
#include <trogdor/lua/luastate.h>
#include <trogdor/iostream/nullout.h>
#include <trogdor/iostream/nullerr.h>

#include <trogdor/entities/room.h>

#include "testluastate.h"


// Runs basic sanity checks on new instances of LuaState
const char *SANITY_CHECK = "sanitycheck.lua";

// Tests luaL_checkudata_ex()
const char *LUAL_CHECKUDATA_EX_TEST = "test_lual_checkudata_ex.lua";

// Lua function that returns true.
int luaReturnTrue(lua_State *L) {

    lua_pushboolean(L, true);
    return 1;
}

// Tests luaL_checkudata_ex() by taking as input an entity and a type and
// returning true if an argument of that type could be read from the stack and
// false if not.
int luaTestCheckudataex(lua_State *L) {

    bool success = false;
    std::string errorMsg;
    const char **types = nullptr;

    int n = lua_gettop(L);

    if (n != 2) {
        return luaL_error(L, "function requires two arguments: entity and type (string)");
    }

    std::string type = lua_tostring(L, -1);

    switch (trogdor::entity::Entity::strToType(type)) {

        case trogdor::entity::ENTITY_ENTITY:

            types = trogdor::entity::LuaEntity::types;
            break;

        case trogdor::entity::ENTITY_RESOURCE:

            types = trogdor::entity::LuaResource::types;
            break;

        case trogdor::entity::ENTITY_TANGIBLE:

            types = trogdor::entity::LuaTangible::types;
            break;

        case trogdor::entity::ENTITY_PLACE:

            types = trogdor::entity::LuaPlace::types;
            break;

        case trogdor::entity::ENTITY_ROOM:

            types = trogdor::entity::LuaRoom::types;
            break;

        case trogdor::entity::ENTITY_THING:

            types = trogdor::entity::LuaThing::types;
            break;

        case trogdor::entity::ENTITY_OBJECT:

            types = trogdor::entity::LuaObject::types;
            break;

        case trogdor::entity::ENTITY_BEING:

            types = trogdor::entity::LuaBeing::types;
            break;

        case trogdor::entity::ENTITY_PLAYER:

            types = trogdor::entity::LuaPlayer::types;
            break;

        case trogdor::entity::ENTITY_CREATURE:

            types = trogdor::entity::LuaCreature::types;
            break;

        // Undefined type. Count it as a failure.
        default:
            lua_pushboolean(L, false);
            return 1;
    }

    if (trogdor::LuaState::luaL_checkudata_ex(L, -2, types)) {
        success = true;
    }

    lua_pushboolean(L, success);
    return 1;
}


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
