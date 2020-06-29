#include <doctest.h>
#include <trogdor/iostream/nullerr.h>

#include "../mock/mocktimerjob.h"


TEST_SUITE("Timer (timer/timer.cpp)") {

	TEST_CASE("Timer (timer/timer.cpp): Sane construction") {

		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());
		trogdor::Timer mockTimer(&mockGame);

		CHECK(!mockTimer.isActive());
		CHECK(0 == mockTimer.getTime());
	}

	TEST_CASE("Timer (timer/timer.cpp): start() and stop()") {

		static std::chrono::milliseconds threadSleepTime(TIMER_TICK_MILLISECONDS * 2);

		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());
		trogdor::Timer mockTimer(&mockGame);

		mockTimer.start();
		std::this_thread::sleep_for(threadSleepTime);

		size_t curTime = mockTimer.getTime();

		mockTimer.stop();
		std::this_thread::sleep_for(threadSleepTime);

		size_t updatedTime = mockTimer.getTime();

		// We're testing to ensure that the timer advances while started and
		// that it does not while stopped.
		CHECK(curTime > 0);
		CHECK(curTime == updatedTime);
	}
}
