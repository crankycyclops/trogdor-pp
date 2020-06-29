#include <doctest.h>

#include <trogdor/timer/jobs/wander.h>
#include <trogdor/entities/creature.h>

#include <trogdor/iostream/nullin.h>
#include <trogdor/iostream/nullout.h>
#include <trogdor/iostream/nullerr.h>


TEST_SUITE("Creature Wander Timer Job (timer/jobs/wander.cpp)") {

	TEST_CASE("Creature Wander Timer Job (timer/jobs/wander.cpp): Wandering is disabled") {

		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());

		trogdor::entity::Creature wanderer(
			&mockGame,
			"wanderer",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		wanderer.setWanderEnabled(false);

		trogdor::Timer mockTimer(&mockGame);
		trogdor::WanderTimerJob job(&mockGame, 1, -1, 1, &wanderer);

		job.execute();

		// If a Creature isn't allowed to wander, the timer job should remove
		// itself so it doesn't try executing again.
		CHECK(0 == job.getExecutions());
	}

	TEST_CASE("Creature Wander Timer Job (timer/jobs/wander.cpp): Wandering is enabled, interval doesn't change") {

		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());

		trogdor::entity::Creature wanderer(
			&mockGame,
			"wanderer",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		wanderer.setWanderEnabled(true);

		trogdor::Timer mockTimer(&mockGame);
		trogdor::WanderTimerJob job(&mockGame, 1, -1, 1, &wanderer);

		job.execute();
		CHECK(-1 == job.getExecutions());
	}

	TEST_CASE("Creature Wander Timer Job (timer/jobs/wander.cpp): Wandering is enabled, interval changes") {

		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());

		trogdor::entity::Creature wanderer(
			&mockGame,
			"wanderer",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		wanderer.setWanderEnabled(true);
		wanderer.setWanderInterval(5);

		trogdor::Timer mockTimer(&mockGame);
		trogdor::WanderTimerJob job(&mockGame, 1, -1, 1, &wanderer);

		job.execute();

		CHECK(-1 == job.getExecutions());
		CHECK(5 == job.getInterval());
	}
}
