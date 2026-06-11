#include <doctest.h>

#include <memory>

#include <trogdor/game.h>
#include <trogdor/lua/luastate.h>
#include <trogdor/exception/luaexception.h>

#include <trogdor/entities/room.h>

#include <trogdor/iostream/nullout.h>
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

	// Lua entity userdata used to store a raw Entity pointer, so a script that
	// held a game owned entity that was later destroyed outside Lua would then
	// go on to dereference a dangling pointer. This regression test ensures that
	// bug remains fixed.
	TEST_CASE("LuaGame (lua/api/luagame.cpp): a script's reference to a game owned entity is invalidated when the Game frees it") {

		SUBCASE("Using a stale game owned reference raises a catchable error instead of dereferencing a dangling pointer") {

			std::unique_ptr<trogdor::Game> game = std::make_unique<trogdor::Game>(
				std::make_unique<trogdor::NullErr>()
			);

			// The Game is the owner of this Room, not Lua, so removeEntity()
			// below should actually free it
			game->insertEntity("r", std::make_shared<trogdor::entity::Room>(
				game.get(),
				"r",
				std::make_unique<trogdor::NullOut>(),
				std::make_unique<trogdor::NullErr>()
			));

			TestLuaState L(game.get());

			L.loadScriptFromString(
				"function stash()\n"
				"   stashed = game:getEntity('r')\n"
				"end\n"
				"function useStashed()\n"
				"   return stashed:getName()\n"
				"end\n"
			);

			// Stash the game owned entity in a Lua global during one event...
			L.call("stash");
			L.execute(0);

			// ...and while it's still alive, the script can use it normally.
			L.call("useStashed");
			L.execute(1);
			CHECK(L.getString(0) == "r");

			// The Game later frees the entity outside Lua
			game->removeEntity("r");

			// Touching the now stale reference should raise a catchable
			// LuaException rather than attempting to dereference freed memory
			L.call("useStashed");
			CHECK_THROWS_AS(L.execute(1), trogdor::LuaException);
		}

		SUBCASE("Garbage collecting a stale game owned reference does not free an already freed entity") {

			std::unique_ptr<trogdor::Game> game = std::make_unique<trogdor::Game>(
				std::make_unique<trogdor::NullErr>()
			);

			game->insertEntity("r", std::make_shared<trogdor::entity::Room>(
				game.get(),
				"r",
				std::make_unique<trogdor::NullOut>(),
				std::make_unique<trogdor::NullErr>()
			));

			TestLuaState L(game.get());

			L.loadScriptFromString(
				"function stash()\n"
				"   stashed = game:getEntity('r')\n"
				"end\n"
				"function drop()\n"
				"   stashed = nil\n"
				"end\n"
			);

			L.call("stash");
			L.execute(0);

			// Free the entity in the Game, then drop the Lua reference and force
			// a full collection. gcEntity must NOT delete a GAME_OWNED entity,
			// which in this case would be a dangling pointer and cause a double
			// free.
			game->removeEntity("r");
			L.call("drop");
			L.execute(0);
			lua_gc(L.getRealState(), LUA_GCCOLLECT, 0);

			CHECK(game->getEntity("r") == nullptr);
		}

		SUBCASE("An entity kept after game:insert() is also invalidated when the Game frees it") {

			std::unique_ptr<trogdor::Game> game = std::make_unique<trogdor::Game>(
				std::make_unique<trogdor::NullErr>()
			);

			TestLuaState L(game.get());

			// Create the entity in Lua (initially LUA_OWNED), hand it to the
			// Game, and keep the variable. game:insert() must retag the
			// userdata as GAME_OWNED so the kept reference is validated rather
			// than trusting a raw pointer the Game can free.
			L.loadScriptFromString(
				"function makeAndKeep()\n"
				"   kept = Room.new('r')\n"
				"   game:insert(kept)\n"
				"end\n"
				"function useKept()\n"
				"   return kept:getName()\n"
				"end\n"
			);

			L.call("makeAndKeep");
			L.execute(0);

			L.call("useKept");
			L.execute(1);
			CHECK(L.getString(0) == "r");

			game->removeEntity("r");

			L.call("useKept");
			CHECK_THROWS_AS(L.execute(1), trogdor::LuaException);
		}
	}
}
