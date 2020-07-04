#include <doctest.h>

#include <trogdor/event/triggers/respawn.h>

#include <trogdor/entities/creature.h>

#include <trogdor/iostream/nullin.h>
#include <trogdor/iostream/nullout.h>
#include <trogdor/iostream/nullerr.h>


TEST_SUITE("RespawnEventTrigger (event/triggers/respawn.cpp)") {

	TEST_CASE("RespawnEventTrigger (event/triggers/respawn.cpp): Respawn disabled, Creature is alive") {

		SUBCASE("Respawn interval = 0 (respawns right away)") {

			trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());

			std::shared_ptr<trogdor::entity::Creature> aliveGuy =
			std::make_shared<trogdor::entity::Creature>(
				&mockGame,
				"dead_guy",
				std::make_unique<trogdor::NullOut>(),
				std::make_unique<trogdor::NullErr>()
			);

			// Disable respawn and set interval to 0
			aliveGuy->setRespawnEnabled(false);
			aliveGuy->setRespawnInterval(0);

			aliveGuy->setHealth(8), aliveGuy->setMaxHealth(10);

			CHECK(aliveGuy->isAlive());

			trogdor::event::RespawnEventTrigger respawnTrigger;
			auto result = respawnTrigger({"test", {}, {&mockGame, aliveGuy.get()}});

			// The Creature should have remained alive with its health the same
			CHECK(result.allowAction);
			CHECK(result.continueExecution);
			CHECK(aliveGuy->isAlive());
			CHECK(8 == aliveGuy->getHealth());
			CHECK(10 == aliveGuy->getMaxHealth());
		}

		SUBCASE("Respawn interval > 0") {

			// TODO
			// Make sure to call aliveGuy->setRespawnInterval(1);
		}
	}

	TEST_CASE("RespawnEventTrigger (event/triggers/respawn.cpp): Respawn disabled, Creature is dead") {

		SUBCASE("Respawn interval = 0 (respawns right away)") {

			trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());

			std::shared_ptr<trogdor::entity::Creature> deadGuy =
			std::make_shared<trogdor::entity::Creature>(
				&mockGame,
				"dead_guy",
				std::make_unique<trogdor::NullOut>(),
				std::make_unique<trogdor::NullErr>()
			);

			// Disable respawn and set interval to 0
			deadGuy->setRespawnEnabled(false);
			deadGuy->setRespawnInterval(0);

			// Until we set a max health, a Being is considered immortal
			deadGuy->setHealth(10), deadGuy->setMaxHealth(10);
			deadGuy->die();

			CHECK(!deadGuy->isAlive());

			trogdor::event::RespawnEventTrigger respawnTrigger;
			auto result = respawnTrigger({"test", {}, {&mockGame, deadGuy.get()}});

			// The Creature should have remained dead
			CHECK(result.allowAction);
			CHECK(result.continueExecution);
			CHECK(!deadGuy->isAlive());
		}

		SUBCASE("Respawn interval > 0") {

			// TODO
			// Make sure to call deadGuy->setRespawnInterval(1);
		}
	}

	TEST_CASE("RespawnEventTrigger (event/triggers/respawn.cpp): Respawn enabled, Creature is alive") {

		SUBCASE("Respawn interval = 0 (respawns right away)") {

			trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());

			std::shared_ptr<trogdor::entity::Creature> aliveGuy =
			std::make_shared<trogdor::entity::Creature>(
				&mockGame,
				"dead_guy",
				std::make_unique<trogdor::NullOut>(),
				std::make_unique<trogdor::NullErr>()
			);

			// Enable respawn and set interval to 0
			aliveGuy->setRespawnEnabled(true);
			aliveGuy->setRespawnInterval(0);

			aliveGuy->setHealth(8), aliveGuy->setMaxHealth(10);

			CHECK(aliveGuy->isAlive());

			trogdor::event::RespawnEventTrigger respawnTrigger;
			auto result = respawnTrigger({"test", {}, {&mockGame, aliveGuy.get()}});

			// The Creature should have remained alive with its health the same
			CHECK(result.allowAction);
			CHECK(result.continueExecution);
			CHECK(aliveGuy->isAlive());
			CHECK(8 == aliveGuy->getHealth());
			CHECK(10 == aliveGuy->getMaxHealth());
		}

		SUBCASE("Respawn interval > 0") {

			// TODO
			// Make sure to call aliveGuy->setRespawnInterval(1);
		}
	}

	TEST_CASE("RespawnEventTrigger (event/triggers/respawn.cpp): Respawn enabled, Creature is dead") {

		SUBCASE("Respawn interval = 0 (respawns right away)") {

			trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());

			std::shared_ptr<trogdor::entity::Creature> deadGuy =
			std::make_shared<trogdor::entity::Creature>(
				&mockGame,
				"dead_guy",
				std::make_unique<trogdor::NullOut>(),
				std::make_unique<trogdor::NullErr>()
			);

			// Enable respawn and set interval to 0
			deadGuy->setRespawnEnabled(true);
			deadGuy->setRespawnInterval(0);

			// Until we set a max health, a Being is considered immortal
			deadGuy->setHealth(10), deadGuy->setMaxHealth(10);
			deadGuy->die();

			CHECK(!deadGuy->isAlive());

			trogdor::event::RespawnEventTrigger respawnTrigger;
			auto result = respawnTrigger({"test", {}, {&mockGame, deadGuy.get()}});

			// The Creature should have respawned right away
			CHECK(result.allowAction);
			CHECK(result.continueExecution);
			CHECK(deadGuy->isAlive());
		}

		SUBCASE("Respawn interval > 0") {

			// TODO
			// Make sure to call deadGuy->setRespawnInterval(1);
		}
	}
}
