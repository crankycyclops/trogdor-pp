#include <doctest.h>

#include <memory>

#include <trogdor/game.h>
#include <trogdor/lua/luastate.h>
#include <trogdor/exception/luaexception.h>

#include <trogdor/entities/creature.h>
#include <trogdor/entities/object.h>

#include <trogdor/iostream/nullerr.h>


TEST_SUITE("LuaBeing (lua/api/entities/luabeing.cpp)") {

	TEST_CASE("LuaBeing (lua/api/entities/luabeing.cpp): insertIntoInventory requires both the Being and the Object to be owned by the Game") {

		SUBCASE("Both entities are created by Lua and NOT inserted into the game: rejected") {

			std::unique_ptr<trogdor::Game> game = std::make_unique<trogdor::Game>(
				std::make_unique<trogdor::NullErr>()
			);

			trogdor::LuaState L(game.get());

			L.loadScriptFromString(
				"function tryInsert()\n"
				"   local c = Creature.new('c')\n"
				"   local o = Object.new('o')\n"
				"   Being.insertIntoInventory(false, o, c)\n"
				"end\n"
			);

			L.call("tryInsert");
			CHECK_THROWS_AS(L.execute(0), trogdor::LuaException);
		}

		SUBCASE("Being is owned by the Game but the Object is not: rejected") {

			std::unique_ptr<trogdor::Game> game = std::make_unique<trogdor::Game>(
				std::make_unique<trogdor::NullErr>()
			);

			trogdor::LuaState L(game.get());

			L.loadScriptFromString(
				"function tryInsert()\n"
				"   local c = Creature.new('c')\n"
				"   local o = Object.new('o')\n"
				"   game:insert(c)\n"
				"   Being.insertIntoInventory(false, o, c)\n"
				"end\n"
			);

			L.call("tryInsert");
			CHECK_THROWS_AS(L.execute(0), trogdor::LuaException);
		}

		SUBCASE("Both entities are owned by the Game: call succeeds") {

			std::unique_ptr<trogdor::Game> game = std::make_unique<trogdor::Game>(
				std::make_unique<trogdor::NullErr>()
			);

			trogdor::LuaState L(game.get());

			// considerWeight is false so the insert can never be rejected for being too heavy
			L.loadScriptFromString(
				"function tryInsert()\n"
				"   local c = Creature.new('c')\n"
				"   local o = Object.new('o')\n"
				"   game:insert(c)\n"
				"   game:insert(o)\n"
				"   Being.insertIntoInventory(false, o, c)\n"
				"end\n"
			);

			L.call("tryInsert");
			CHECK_NOTHROW(L.execute(0));
		}
	}

	TEST_CASE("LuaBeing (lua/api/entities/luabeing.cpp): removeFromInventory requires both the Being and the Object to be owned by the Game") {

		SUBCASE("Both entities are created by Lua and are NOT inserted into the game: rejected") {

			std::unique_ptr<trogdor::Game> game = std::make_unique<trogdor::Game>(
				std::make_unique<trogdor::NullErr>()
			);

			trogdor::LuaState L(game.get());

			L.loadScriptFromString(
				"function tryRemove()\n"
				"   local c = Creature.new('c')\n"
				"   local o = Object.new('o')\n"
				"   Being.removeFromInventory(o, c)\n"
				"end\n"
			);

			L.call("tryRemove");
			CHECK_THROWS_AS(L.execute(0), trogdor::LuaException);
		}

		SUBCASE("Both entities are owned by the Game: call succeeds") {

			std::unique_ptr<trogdor::Game> game = std::make_unique<trogdor::Game>(
				std::make_unique<trogdor::NullErr>()
			);

			trogdor::LuaState L(game.get());

			L.loadScriptFromString(
				"function insertThenRemove()\n"
				"   local c = Creature.new('c')\n"
				"   local o = Object.new('o')\n"
				"   game:insert(c)\n"
				"   game:insert(o)\n"
				"   Being.insertIntoInventory(false, o, c)\n"
				"   Being.removeFromInventory(o, c)\n"
				"end\n"
			);

			L.call("insertThenRemove");
			CHECK_NOTHROW(L.execute(0));
		}
	}
}
