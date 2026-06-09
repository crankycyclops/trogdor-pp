#include <doctest.h>

#include <trogdor/entities/creature.h>
#include <trogdor/timer/jobs/respawn.h>

#include <trogdor/iostream/nullout.h>
#include <trogdor/iostream/nullerr.h>


TEST_SUITE("Being Respawn Timer Job (timer/jobs/respawn.cpp)") {

	TEST_CASE("Being Respawn Timer Job (timer/jobs/respawn.cpp): Respawn dead guy") {

		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());

		std::shared_ptr<trogdor::entity::Creature> deadGuy =
		std::make_shared<trogdor::entity::Creature>(
			&mockGame,
			"dead_guy",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		// Until we set a max health, a Being is considered immortal
		deadGuy->setProperty(trogdor::entity::Being::HealthProperty, 10);
		deadGuy->setProperty(trogdor::entity::Being::MaxHealthProperty, 10);
		deadGuy->die();

		CHECK(!deadGuy->isAlive());

		trogdor::Timer mockTimer(&mockGame);
		trogdor::RespawnTimerJob job(&mockGame, 1, 1, 1, deadGuy);

		// Resurrection!
		job.execute();
		CHECK(deadGuy->isAlive());
	}

	TEST_CASE("Being Respawn Timer Job (timer/jobs/respawn.cpp): Respawn guy who's already alive") {

		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());

		std::shared_ptr<trogdor::entity::Creature> deadGuy =
		std::make_shared<trogdor::entity::Creature>(
			&mockGame,
			"alive_guy",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		// Until we set a max health, a Being is considered immortal
		deadGuy->setProperty(trogdor::entity::Being::HealthProperty, 10);
		deadGuy->setProperty(trogdor::entity::Being::MaxHealthProperty, 10);

		CHECK(deadGuy->isAlive());

		trogdor::Timer mockTimer(&mockGame);
		trogdor::RespawnTimerJob job(&mockGame, 1, 1, 1, deadGuy);

		// Not necessary, but thanks anyway!
		job.execute();
		CHECK(deadGuy->isAlive());
	}

	TEST_CASE("Being Respawn Timer Job (timer/jobs/respawn.cpp): Dead guy removed before execution (make sure we don't dereference a dangling pointer)") {

		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());

		std::shared_ptr<trogdor::entity::Creature> deadGuy =
		std::make_shared<trogdor::entity::Creature>(
			&mockGame,
			"dead_guy",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		deadGuy->setProperty(trogdor::entity::Being::HealthProperty, 10);
		deadGuy->setProperty(trogdor::entity::Being::MaxHealthProperty, 10);
		deadGuy->die();

		trogdor::Timer mockTimer(&mockGame);
		trogdor::RespawnTimerJob job(&mockGame, 1, 1, 1, deadGuy);

		// Simulate the Being being removed from the game before the job runs.
		// The job holds only a std::weak_ptr, so execute() must expire the
		// job instead of dereferencing freed memory.
		deadGuy = nullptr;

		job.execute();

		CHECK(0 == job.getExecutions());
	}
}
