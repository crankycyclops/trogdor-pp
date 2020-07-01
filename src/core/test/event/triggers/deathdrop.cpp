#include <doctest.h>

#include <trogdor/event/triggers/deathdrop.h>

#include <trogdor/entities/room.h>
#include <trogdor/entities/object.h>
#include <trogdor/entities/creature.h>

#include <trogdor/iostream/nullin.h>
#include <trogdor/iostream/nullout.h>
#include <trogdor/iostream/nullerr.h>

TEST_SUITE("DeathDropEventTrigger (event/triggers/deathdrop.cpp)") {

	// Their stuff should NOT be dropped
	TEST_CASE("DeathDropEventTrigger (event/triggers/deathdrop.cpp): Being is alive") {

		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());

		trogdor::entity::Creature aliveGuy(
			&mockGame,
			"dead_guy",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		// TODO: finish and confirm their items didn't drop
	}

	// Their stuff SHOULD be dropped
	TEST_CASE("DeathDropEventTrigger (event/triggers/deathdrop.cpp): Being is dead") {

		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());

		trogdor::entity::Creature deadGuy(
			&mockGame,
			"dead_guy",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		// Until we set a max health, a Being is considered immortal
		deadGuy.setHealth(10), deadGuy.setMaxHealth(10);
		deadGuy.die();

		// TODO: finish and confirm their items did drop
	}
}
