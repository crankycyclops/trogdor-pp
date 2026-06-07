#include <doctest.h>

#include <memory>

#include <trogdor/game.h>
#include <trogdor/lua/luastate.h>

#include <trogdor/entities/room.h>

#include <trogdor/iostream/nullerr.h>

#include "testluastate.h"


TEST_SUITE("LuaGame (lua/api/luagame.cpp)") {

	TEST_CASE("LuaGame (lua/api/luagame.cpp): game:insert() properly transfers ownership away from Lua") {

		std::unique_ptr<trogdor::Game> game = std::make_unique<trogdor::Game>(
			std::make_unique<trogdor::NullErr>()
		);

		TestLuaState L(game.get());

		L.loadScriptFromString(
			"function insertRoom(name)\n"
			"    local r = Room.new(name)\n"
			"    return game:insert(r)\n"
			"end\n"
		);

		// Create a Room from within Lua and insert it into the game
		L.call("insertRoom");
		L.pushArgument(std::string("r"));
		L.execute(1);
		CHECK(L.getBoolean(0)); // insert succeeded

		// The game now owns the entity and ownership has been transferred away
		// from Lua
		auto entity = game->getEntity("r");
		REQUIRE(entity != nullptr);
		CHECK(entity->getName() == "r");
		CHECK(!entity->isManagedByLua());

		// Force a full Lua garbage collection. Under a bug that inspired this
		// test case, the room was still flagged managedByLua, so __gc would
		// delete the game owned entity here and leave a dangling pointer. With
		// the fix I applied, the entity should now survive untouched.
		lua_gc(L.getRealState(), LUA_GCCOLLECT, 0);

		auto afterGC = game->getEntity("r");
		REQUIRE(afterGC != nullptr);
		CHECK(afterGC->getName() == "r");
		CHECK(!afterGC->isManagedByLua());

		// A duplicate insert must be rejected (return false) without transferring
		// ownership of, or freeing, anything
		L.call("insertRoom");
		L.pushArgument(std::string("r"));
		L.execute(1);
		CHECK(!L.getBoolean(0)); // duplicate rejected

		// The original entity is still owned by the game and intact
		auto original = game->getEntity("r");
		REQUIRE(original != nullptr);
		CHECK(original->getName() == "r");
	}
}
