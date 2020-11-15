#include <doctest.h>

#include <atomic>
#include <chrono>
#include <condition_variable>

#include <trogdor/event/triggers/respawn.h>

#include <trogdor/entities/creature.h>

#include <trogdor/iostream/nullout.h>
#include <trogdor/iostream/nullerr.h>

#include "../../mock/mocktimerjob.h"

using namespace std::chrono_literals;


TEST_SUITE("RespawnEventTrigger (event/triggers/respawn.cpp)") {

	TEST_CASE("RespawnEventTrigger (event/triggers/respawn.cpp): Respawn disabled, Creature is alive") {

		SUBCASE("Respawn interval = 0 (respawns right away)") {

			trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());

			std::shared_ptr<trogdor::entity::Creature> aliveGuy =
			std::make_shared<trogdor::entity::Creature>(
				&mockGame,
				"alive_guy",
				std::make_unique<trogdor::NullOut>(),
				std::make_unique<trogdor::NullErr>()
			);

			// Disable respawn and set interval to 0
			aliveGuy->setProperty(trogdor::entity::Being::RespawnEnabledProperty, false);
			aliveGuy->setProperty(trogdor::entity::Being::RespawnIntervalProperty, static_cast<int>(0));

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

			trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());

			std::shared_ptr<trogdor::entity::Creature> aliveGuy =
			std::make_shared<trogdor::entity::Creature>(
				&mockGame,
				"alive_guy",
				std::make_unique<trogdor::NullOut>(),
				std::make_unique<trogdor::NullErr>()
			);

			// Disable respawn and set interval to 0
			aliveGuy->setProperty(trogdor::entity::Being::RespawnEnabledProperty, false);
			aliveGuy->setProperty(trogdor::entity::Being::RespawnIntervalProperty, 1);

			aliveGuy->setHealth(8), aliveGuy->setMaxHealth(10);

			CHECK(aliveGuy->isAlive());

			auto now = std::chrono::system_clock::now();

			trogdor::event::RespawnEventTrigger respawnTrigger;
			auto result = respawnTrigger({"test", {}, {&mockGame, aliveGuy.get()}});

			CHECK(result.allowAction);
			CHECK(result.continueExecution);

			// We'll wait until we're sure the Timer has had time to execute
			// the respawn job (assuming it was inserted, which it shouldn't
			// actually have been.)
			std::atomic<int> wakeup{0};
			std::condition_variable timerDone;
			std::mutex mutex;
			std::unique_lock<std::mutex> lock(mutex);

			mockGame.insertTimerJob(std::make_shared<MockTimerJob>(
				&mockGame, 1, 2, 0, [&]() {

					static int numExecutions = 1;

					if (numExecutions) {
						numExecutions--;
					}

					// The respawn timer job should have executed by now
					else {
						wakeup = 1;
						timerDone.notify_all();
					}
				}
			));

			mockGame.start();

			if (!timerDone.wait_until(lock, now + 30ms, [&](){return wakeup == 1;})) {
				// If we get here, it means our mock timer job didn't
				// execute like it was supposed to
				CHECK(false);
			}

			mockGame.stop();

			// The Creature should remain alive
			CHECK(aliveGuy->isAlive());
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
			deadGuy->setProperty(trogdor::entity::Being::RespawnEnabledProperty, false);
			deadGuy->setProperty(trogdor::entity::Being::RespawnIntervalProperty, static_cast<int>(0));

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

			trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());

			std::shared_ptr<trogdor::entity::Creature> deadGuy =
			std::make_shared<trogdor::entity::Creature>(
				&mockGame,
				"dead_guy",
				std::make_unique<trogdor::NullOut>(),
				std::make_unique<trogdor::NullErr>()
			);

			// Disable respawn and set interval to 0
			deadGuy->setProperty(trogdor::entity::Being::RespawnEnabledProperty, false);
			deadGuy->setProperty(trogdor::entity::Being::RespawnIntervalProperty, 1);

			// Until we set a max health, a Being is considered immortal
			deadGuy->setHealth(10), deadGuy->setMaxHealth(10);
			deadGuy->die();

			CHECK(!deadGuy->isAlive());

			trogdor::event::RespawnEventTrigger respawnTrigger;
			auto result = respawnTrigger({"test", {}, {&mockGame, deadGuy.get()}});

			CHECK(result.allowAction);
			CHECK(result.continueExecution);

			auto now = std::chrono::system_clock::now();

			// We'll wait until we're sure the Timer has had time to execute
			// the respawn job (assuming it was inserted, which it shouldn't
			// actually have been.)
			std::atomic<int> wakeup{0};
			std::condition_variable timerDone;
			std::mutex mutex;
			std::unique_lock<std::mutex> lock(mutex);

			mockGame.insertTimerJob(std::make_shared<MockTimerJob>(
				&mockGame, 1, 2, 0, [&]() {

					static int numExecutions = 1;

					if (numExecutions) {
						numExecutions--;
					}

					// The respawn timer job should have executed by now
					else {
						wakeup = 1;
						timerDone.notify_all();
					}
				}
			));

			mockGame.start();

			if (!timerDone.wait_until(lock, now + 30ms, [&](){return wakeup == 1;})) {
				// If we get here, it means our mock timer job didn't
				// execute like it was supposed to
				CHECK(false);
			}

			mockGame.stop();

			// The Creature should still be dead
			CHECK(!deadGuy->isAlive());
		}
	}

	TEST_CASE("RespawnEventTrigger (event/triggers/respawn.cpp): Respawn enabled, Creature is alive") {

		SUBCASE("Respawn interval = 0 (respawns right away)") {

			trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());

			std::shared_ptr<trogdor::entity::Creature> aliveGuy =
			std::make_shared<trogdor::entity::Creature>(
				&mockGame,
				"alive_guy",
				std::make_unique<trogdor::NullOut>(),
				std::make_unique<trogdor::NullErr>()
			);

			// Enable respawn and set interval to 0 (without the static cast,
			// Entity::setProperty() will assume it's a nullptr of type const
			// char *)
			aliveGuy->setProperty(trogdor::entity::Being::RespawnEnabledProperty, true);
			aliveGuy->setProperty(trogdor::entity::Being::RespawnIntervalProperty, static_cast<int>(0));

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

			trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());

			std::shared_ptr<trogdor::entity::Creature> aliveGuy =
			std::make_shared<trogdor::entity::Creature>(
				&mockGame,
				"alive_guy",
				std::make_unique<trogdor::NullOut>(),
				std::make_unique<trogdor::NullErr>()
			);

			// Enable respawn and set interval to 0
			aliveGuy->setProperty(trogdor::entity::Being::RespawnEnabledProperty, true);
			aliveGuy->setProperty(trogdor::entity::Being::RespawnIntervalProperty, 1);

			aliveGuy->setHealth(8), aliveGuy->setMaxHealth(10);

			CHECK(aliveGuy->isAlive());

			auto now = std::chrono::system_clock::now();

			trogdor::event::RespawnEventTrigger respawnTrigger;
			auto result = respawnTrigger({"test", {}, {&mockGame, aliveGuy.get()}});

			CHECK(result.allowAction);
			CHECK(result.continueExecution);

			// We'll wait until we're sure the Timer has had time to execute
			// the respawn job (assuming it was inserted, which it shouldn't
			// actually have been.)
			std::atomic<int> wakeup{0};
			std::condition_variable timerDone;
			std::mutex mutex;
			std::unique_lock<std::mutex> lock(mutex);

			mockGame.insertTimerJob(std::make_shared<MockTimerJob>(
				&mockGame, 1, 2, 0, [&]() {

					static int numExecutions = 1;

					if (numExecutions) {
						numExecutions--;
					}

					// The respawn timer job should have executed by now
					else {
						wakeup = 1;
						timerDone.notify_all();
					}
				}
			));

			mockGame.start();

			if (!timerDone.wait_until(lock, now + 30ms, [&](){return wakeup == 1;})) {
				// If we get here, it means our mock timer job didn't
				// execute like it was supposed to
				CHECK(false);
			}

			mockGame.stop();

			// Make sure the Creature is still alive and has the same health
			// values
			CHECK(aliveGuy->isAlive());
			CHECK(8 == aliveGuy->getHealth());
			CHECK(10 == aliveGuy->getMaxHealth());
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
			deadGuy->setProperty(trogdor::entity::Being::RespawnEnabledProperty, true);
			deadGuy->setProperty(trogdor::entity::Being::RespawnIntervalProperty, static_cast<int>(0));

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

			trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());

			std::shared_ptr<trogdor::entity::Creature> deadGuy =
			std::make_shared<trogdor::entity::Creature>(
				&mockGame,
				"dead_guy",
				std::make_unique<trogdor::NullOut>(),
				std::make_unique<trogdor::NullErr>()
			);

			// Enable respawn and set interval to 0
			deadGuy->setProperty(trogdor::entity::Being::RespawnEnabledProperty, true);
			deadGuy->setProperty(trogdor::entity::Being::RespawnIntervalProperty, 1);

			// Until we set a max health, a Being is considered immortal
			deadGuy->setHealth(10), deadGuy->setMaxHealth(10);
			deadGuy->die();

			CHECK(!deadGuy->isAlive());

			auto now = std::chrono::system_clock::now();

			// The respawn event trigger should insert a timer job that will
			// respawn the Creature after the defined interval of 1 tick
			trogdor::event::RespawnEventTrigger respawnTrigger;
			auto result = respawnTrigger({"test", {}, {&mockGame, deadGuy.get()}});

			CHECK(result.allowAction);
			CHECK(result.continueExecution);

			// We'll wait until we're sure the Timer has had time to execute
			// the respawn job inserted by the event trigger
			std::atomic<int> wakeup{0};
			std::condition_variable timerDone;
			std::mutex mutex;
			std::unique_lock<std::mutex> lock(mutex);

			mockGame.insertTimerJob(std::make_shared<MockTimerJob>(
				&mockGame, 1, 2, 0, [&]() {

					static int numExecutions = 1;

					if (numExecutions) {
						numExecutions--;
					}

					// The respawn timer job should have executed by now
					else {
						wakeup = 1;
						timerDone.notify_all();
					}
				}
			));

			mockGame.start();

			if (!timerDone.wait_until(lock, now + 30ms, [&](){return wakeup == 1;})) {
				// If we get here, it means our mock timer job didn't
				// execute like it was supposed to
				CHECK(false);
			}

			mockGame.stop();

			// Make sure the Creature respawned after the set interval
			CHECK(deadGuy->isAlive());
		}
	}
}
