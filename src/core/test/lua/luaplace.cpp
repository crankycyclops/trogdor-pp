#include <doctest.h>

#include <memory>

#include <trogdor/game.h>
#include <trogdor/lua/luastate.h>
#include <trogdor/exception/luaexception.h>

#include <trogdor/entities/room.h>
#include <trogdor/entities/object.h>

#include <trogdor/iostream/nullerr.h>


TEST_SUITE("LuaPlace (lua/api/entities/luaplace.cpp)") {

	TEST_CASE("LuaPlace (lua/api/entities/luaplace.cpp): insertThing requires both the Place and the Thing to be owned by the Game") {

		SUBCASE("Both entities are created by Lua and are NOT inserted into the game: rejected") {

			std::unique_ptr<trogdor::Game> game = std::make_unique<trogdor::Game>(
				std::make_unique<trogdor::NullErr>()
			);

			trogdor::LuaState L(game.get());

			L.loadScriptFromString(
				"function tryInsert()\n"
				"   local r = Room.new('r')\n"
				"   local o = Object.new('o')\n"
				"   r:insertThing(o)\n"
				"end\n"
			);

			L.call("tryInsert");
			CHECK_THROWS_AS(L.execute(0), trogdor::LuaException);
		}

		SUBCASE("Place owned by the Game, Thing uninserted: rejected") {

			std::unique_ptr<trogdor::Game> game = std::make_unique<trogdor::Game>(
				std::make_unique<trogdor::NullErr>()
			);

			trogdor::LuaState L(game.get());

			L.loadScriptFromString(
				"function tryInsert()\n"
				"   local r = Room.new('r')\n"
				"   local o = Object.new('o')\n"
				"   game:insert(r)\n"
				"   r:insertThing(o)\n"
				"end\n"
			);

			L.call("tryInsert");
			CHECK_THROWS_AS(L.execute(0), trogdor::LuaException);
		}

		SUBCASE("Thing owned by the Game, Place uninserted: rejected") {

			std::unique_ptr<trogdor::Game> game = std::make_unique<trogdor::Game>(
				std::make_unique<trogdor::NullErr>()
			);

			trogdor::LuaState L(game.get());

			L.loadScriptFromString(
				"function tryInsert()\n"
				"   local r = Room.new('r')\n"
				"   local o = Object.new('o')\n"
				"   game:insert(o)\n"
				"   r:insertThing(o)\n"
				"end\n"
			);

			L.call("tryInsert");
			CHECK_THROWS_AS(L.execute(0), trogdor::LuaException);
		}

		SUBCASE("Both entities owned by the Game: call succeeds") {

			std::unique_ptr<trogdor::Game> game = std::make_unique<trogdor::Game>(
				std::make_unique<trogdor::NullErr>()
			);

			trogdor::LuaState L(game.get());

			L.loadScriptFromString(
				"function tryInsert()\n"
				"   local r = Room.new('r')\n"
				"   local o = Object.new('o')\n"
				"   game:insert(r)\n"
				"   game:insert(o)\n"
				"   r:insertThing(o)\n"
				"end\n"
			);

			L.call("tryInsert");
			CHECK_NOTHROW(L.execute(0));
		}
	}

	TEST_CASE("LuaPlace (lua/api/entities/luaplace.cpp): removeThing requires both the Place and the Thing to be owned by the Game") {

		SUBCASE("Both entities are created by Lua and are NOT inserted into the game: rejected") {

			std::unique_ptr<trogdor::Game> game = std::make_unique<trogdor::Game>(
				std::make_unique<trogdor::NullErr>()
			);

			trogdor::LuaState L(game.get());

			L.loadScriptFromString(
				"function tryRemove()\n"
				"   local r = Room.new('r')\n"
				"   local o = Object.new('o')\n"
				"   r:removeThing(o)\n"
				"end\n"
			);

			L.call("tryRemove");
			CHECK_THROWS_AS(L.execute(0), trogdor::LuaException);
		}

		SUBCASE("Both entities owned by the Game: call succeeds") {

			std::unique_ptr<trogdor::Game> game = std::make_unique<trogdor::Game>(
				std::make_unique<trogdor::NullErr>()
			);

			trogdor::LuaState L(game.get());

			L.loadScriptFromString(
				"function insertThenRemove()\n"
				"   local r = Room.new('r')\n"
				"   local o = Object.new('o')\n"
				"   game:insert(r)\n"
				"   game:insert(o)\n"
				"   r:insertThing(o)\n"
				"   r:removeThing(o)\n"
				"end\n"
			);

			L.call("insertThenRemove");
			CHECK_NOTHROW(L.execute(0));
		}
	}
}
