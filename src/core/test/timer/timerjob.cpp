#include <doctest.h>
#include <trogdor/iostream/nullerr.h>

#include "../mock/mocktimerjob.h"


// Test suite for the TimerJob abstract class
TEST_SUITE("TimerJob (timer/timerjob.cpp)") {

	TEST_CASE("TimerJob (timer/timerjob.cpp): Initialization and getter methods") {

		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());
		trogdor::Timer mockTimer(&mockGame);

		MockTimerJob mockJob(&mockGame, 1, 2, 3, [] {});

		CHECK(0 == mockJob.getInitTime());
		CHECK(1 == mockJob.getInterval());
		CHECK(2 == mockJob.getExecutions());
		CHECK(3 == mockJob.getStartTime());
	}

	TEST_CASE("TimerJob (timer/timerjob.cpp): Setter methods") {

		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());
		trogdor::Timer mockTimer(&mockGame);

		MockTimerJob mockJob(&mockGame, 1, 2, 3, [] {});

		mockJob.setInterval(4);
		mockJob.setExecutions(5);
		mockJob.setStartTime(6);

		CHECK(4 == mockJob.getInterval());
		CHECK(5 == mockJob.getExecutions());
		CHECK(6 == mockJob.getStartTime());
	}

	TEST_CASE("TimerJob (timer/timerjob.cpp): decExecutions()") {

		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());
		trogdor::Timer mockTimer(&mockGame);

		MockTimerJob mockJob(&mockGame, 1, 2, 3, [] {});

		mockJob.decExecutions();

		// Make sure the number of executions was decremented from 2 to 1
		CHECK(1 == mockJob.getExecutions());
	}

	TEST_CASE("TimerJob (timer/timerjob.cpp): Test setting invalid interval") {

		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());
		trogdor::Timer mockTimer(&mockGame);

		try {
			MockTimerJob mockJob(&mockGame, 0, 2, 3, [] {});
			FAIL("Shouldn't be able to set an interval of 0");
		}

		catch (const trogdor::UndefinedException &e) {
			CHECK(true);
		}

		try {
			MockTimerJob mockJob(&mockGame, -1, 2, 3, [] {});
			FAIL("Shouldn't be able to set an interval of -1");
		}

		catch (const trogdor::UndefinedException &e) {
			CHECK(true);
		}
	}
}
