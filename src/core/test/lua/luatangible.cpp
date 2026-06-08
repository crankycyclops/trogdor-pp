#include <doctest.h>

#include <memory>

#include <trogdor/game.h>
#include <trogdor/lua/luastate.h>
#include <trogdor/exception/luaexception.h>

#include <trogdor/entities/creature.h>
#include <trogdor/entities/object.h>

#include <trogdor/iostream/nullerr.h>


TEST_SUITE("LuaTangible (lua/api/entities/luatangible.cpp)") {

	TEST_CASE("LuaTangible (lua/api/entities/luatangible.cpp): observe requires both entities to be owned by the Game") {

		SUBCASE("Both entities are Lua-created and uninserted: rejected") {

			std::unique_ptr<trogdor::Game> game = std::make_unique<trogdor::Game>(
				std::make_unique<trogdor::NullErr>()
			);

			trogdor::LuaState L(game.get());

			L.loadScriptFromString(
				"function tryObserve()\n"
				"   local o = Object.new('o')\n"
				"   local c = Creature.new('c')\n"
				"   Tangible.observe(o, c)\n"
				"end\n"
			);

			L.call("tryObserve");
			CHECK_THROWS_AS(L.execute(0), trogdor::LuaException);
		}

		SUBCASE("Observed owned by the Game but observer is not: rejected") {

			std::unique_ptr<trogdor::Game> game = std::make_unique<trogdor::Game>(
				std::make_unique<trogdor::NullErr>()
			);

			trogdor::LuaState L(game.get());

			L.loadScriptFromString(
				"function tryObserve()\n"
				"   local o = Object.new('o')\n"
				"   local c = Creature.new('c')\n"
				"   game:insert(o)\n"
				"   Tangible.observe(o, c)\n"
				"end\n"
			);

			L.call("tryObserve");
			CHECK_THROWS_AS(L.execute(0), trogdor::LuaException);
		}

		SUBCASE("Both entities owned by the Game: allowed") {

			std::unique_ptr<trogdor::Game> game = std::make_unique<trogdor::Game>(
				std::make_unique<trogdor::NullErr>()
			);

			trogdor::LuaState L(game.get());

			L.loadScriptFromString(
				"function tryObserve()\n"
				"   local o = Object.new('o')\n"
				"   local c = Creature.new('c')\n"
				"   game:insert(o)\n"
				"   game:insert(c)\n"
				"   Tangible.observe(o, c)\n"
				"end\n"
			);

			L.call("tryObserve");
			CHECK_NOTHROW(L.execute(0));
		}
	}

	TEST_CASE("LuaTangible (lua/api/entities/luatangible.cpp): glance requires both entities to be owned by the Game") {

		std::unique_ptr<trogdor::Game> game = std::make_unique<trogdor::Game>(
			std::make_unique<trogdor::NullErr>()
		);

		trogdor::LuaState L(game.get());

		L.loadScriptFromString(
			"function tryGlance()\n"
			"   local o = Object.new('o')\n"
			"   local c = Creature.new('c')\n"
			"   Tangible.glance(o, c)\n"
			"end\n"
		);

		L.call("tryGlance");
		CHECK_THROWS_AS(L.execute(0), trogdor::LuaException);
	}

	TEST_CASE("LuaTangible (lua/api/entities/luatangible.cpp): observedBy requires both entities to be owned by the Game") {

		std::unique_ptr<trogdor::Game> game = std::make_unique<trogdor::Game>(
			std::make_unique<trogdor::NullErr>()
		);

		trogdor::LuaState L(game.get());

		L.loadScriptFromString(
			"function tryObservedBy()\n"
			"   local o = Object.new('o')\n"
			"   local c = Creature.new('c')\n"
			"   return Tangible.observedBy(o, c)\n"
			"end\n"
		);

		L.call("tryObservedBy");
		CHECK_THROWS_AS(L.execute(1), trogdor::LuaException);
	}

	TEST_CASE("LuaTangible (lua/api/entities/luatangible.cpp): glancedBy requires both entities to be owned by the Game") {

		std::unique_ptr<trogdor::Game> game = std::make_unique<trogdor::Game>(
			std::make_unique<trogdor::NullErr>()
		);

		trogdor::LuaState L(game.get());

		L.loadScriptFromString(
			"function tryGlancedBy()\n"
			"   local o = Object.new('o')\n"
			"   local c = Creature.new('c')\n"
			"   return Tangible.glancedBy(o, c)\n"
			"end\n"
		);

		L.call("tryGlancedBy");
		CHECK_THROWS_AS(L.execute(1), trogdor::LuaException);
	}
}
