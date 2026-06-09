#include <doctest.h>

#include <trogdor/timer/jobs/wander.h>
#include <trogdor/entities/creature.h>

#include <trogdor/iostream/nullout.h>
#include <trogdor/iostream/nullerr.h>


TEST_SUITE("Creature Wander Timer Job (timer/jobs/wander.cpp)") {

	TEST_CASE("Creature Wander Timer Job (timer/jobs/wander.cpp): Wandering is disabled") {

		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());

		std::shared_ptr<trogdor::entity::Creature> wanderer =
		std::make_shared<trogdor::entity::Creature>(
			&mockGame,
			"wanderer",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		wanderer->setProperty(trogdor::entity::Creature::WanderEnabledProperty, false);

		trogdor::Timer mockTimer(&mockGame);
		trogdor::WanderTimerJob job(&mockGame, 1, -1, 1, wanderer);

		job.execute();

		// If a Creature isn't allowed to wander, the timer job should remove
		// itself so it doesn't try executing again.
		CHECK(0 == job.getExecutions());
	}

	TEST_CASE("Creature Wander Timer Job (timer/jobs/wander.cpp): Wandering is enabled, interval doesn't change") {

		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());

		std::shared_ptr<trogdor::entity::Creature> wanderer =
		std::make_shared<trogdor::entity::Creature>(
			&mockGame,
			"wanderer",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		wanderer->setProperty(trogdor::entity::Creature::WanderEnabledProperty, true);

		trogdor::Timer mockTimer(&mockGame);
		trogdor::WanderTimerJob job(&mockGame, 1, -1, 1, wanderer);

		job.execute();
		CHECK(-1 == job.getExecutions());
	}

	TEST_CASE("Creature Wander Timer Job (timer/jobs/wander.cpp): Wandering is enabled, interval changes") {

		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());

		std::shared_ptr<trogdor::entity::Creature> wanderer =
		std::make_shared<trogdor::entity::Creature>(
			&mockGame,
			"wanderer",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		wanderer->setProperty(trogdor::entity::Creature::WanderEnabledProperty, true);
		wanderer->setProperty(trogdor::entity::Creature::WanderIntervalProperty, 5);

		trogdor::Timer mockTimer(&mockGame);
		trogdor::WanderTimerJob job(&mockGame, 1, -1, 1, wanderer);

		job.execute();

		CHECK(-1 == job.getExecutions());
		CHECK(5 == job.getInterval());
	}

	TEST_CASE("Creature Wander Timer Job (timer/jobs/wander.cpp): Wanderer removed before execution (make sure we don't try to dereference a dangling pointer)") {

		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());

		std::shared_ptr<trogdor::entity::Creature> wanderer =
		std::make_shared<trogdor::entity::Creature>(
			&mockGame,
			"wanderer",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		wanderer->setProperty(trogdor::entity::Creature::WanderEnabledProperty, true);

		trogdor::Timer mockTimer(&mockGame);
		trogdor::WanderTimerJob job(&mockGame, 1, -1, 1, wanderer);

		// Simulate the Creature being removed from the game before the job
		// runs. The job holds only a std::weak_ptr, so execute() must expire
		// the job instead of dereferencing freed memory.
		wanderer = nullptr;

		job.execute();

		CHECK(0 == job.getExecutions());
	}
}
