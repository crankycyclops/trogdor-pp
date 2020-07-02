#include <doctest.h>
#include <trogdor/iostream/nullerr.h>

#include "../mock/mocktimerjob.h"

// Number of ms between clock ticks (make this value small enough to ensure
// time-dependent unit tests finish quickly, but not so small that the Timer
// becomes unstable.)
constexpr size_t tickInterval = 5;


TEST_SUITE("Timer (timer/timer.cpp)") {

	TEST_CASE("Timer (timer/timer.cpp): Sane construction, isActive(), and getTime()") {

		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());
		trogdor::Timer mockTimer(&mockGame);

		CHECK(!mockTimer.isActive());
		CHECK(0 == mockTimer.getTime());
	}

	TEST_CASE("Timer (timer/timer.cpp): start() and stop()") {

		static std::chrono::milliseconds threadSleepTime(tickInterval * 2);

		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());
		trogdor::Timer mockTimer(&mockGame, tickInterval);

		mockTimer.start();
		std::this_thread::sleep_for(threadSleepTime);

		size_t curTime = mockTimer.getTime();

		mockTimer.stop();
		std::this_thread::sleep_for(threadSleepTime);

		size_t updatedTime = mockTimer.getTime();

		// We're testing to ensure that the timer advances while started and
		// that it does not while stopped.
		CHECK(curTime > 0);

		// Due to imprecisions in the way things are timed, it's possible for
		// the clock to advance one more than the expected value. Note that
		// Doctest only allows me to test binary comparisons, hence the
		// variable.
		bool result = updatedTime == curTime || updatedTime == curTime + 1;
		CHECK(result);
	}

	TEST_CASE("Timer (timer/timer.cpp): deactivate() and shutdown()") {

		static std::chrono::milliseconds threadSleepTime(tickInterval * 2);

		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());
		trogdor::Timer mockTimer(&mockGame, tickInterval);

		mockTimer.start();
		std::this_thread::sleep_for(threadSleepTime);

		size_t curTime = mockTimer.getTime();

		mockTimer.deactivate();
		std::this_thread::sleep_for(threadSleepTime);

		size_t updatedTime = mockTimer.getTime();

		// The way we'll know if this fails is that we'll get a segmentation
		// fault due to an unjoined thread ;)
		mockTimer.shutdown();

		// We're basically just verifying that a call to deactivate() followed
		// by a call to shutdown() is equivalent to a call to stop(). Doctest
		// only allows binary comparisons, hence the variable.
		bool result = updatedTime == curTime || updatedTime == curTime + 1;
		CHECK(result);
	}

	TEST_CASE("Timer (timer/timer.cpp): reset()") {

		SUBCASE("Calling reset() on stopped clock") {

			static std::chrono::milliseconds threadSleepTime(tickInterval * 2);

			trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());
			trogdor::Timer mockTimer(&mockGame, tickInterval);

			mockTimer.start();
			std::this_thread::sleep_for(threadSleepTime);

			mockTimer.stop();
			mockTimer.reset();
			std::this_thread::sleep_for(threadSleepTime);

			CHECK(0 == mockTimer.getTime());
		}

		SUBCASE("Calling reset() on started clock") {

			static std::chrono::milliseconds firstSleepTime(tickInterval * 4);
			static std::chrono::milliseconds secondSleepTime(tickInterval * 2);

			trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());
			trogdor::Timer mockTimer(&mockGame, tickInterval);

			mockTimer.start();
			std::this_thread::sleep_for(firstSleepTime);

			size_t firstTime = mockTimer.getTime();

			mockTimer.reset();
			std::this_thread::sleep_for(secondSleepTime);

			size_t secondTime = mockTimer.getTime();

			mockTimer.stop();

			// By running the clock for a long time, resetting it, and continuing
			// to run it for a shorter time, we can verify that calling reset()
			// on a running clock sets the time back to 0 but that the clock
			// continues ticking.
			CHECK(secondTime > 0);
			CHECK(secondTime < firstTime);
		}
	}

	TEST_CASE("Timer (timer/timer.cpp): insertJob()") {

		SUBCASE("Inserting job into stopped timer") {

			static std::chrono::milliseconds threadSleepTime(tickInterval * 2);

			trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());
			trogdor::Timer mockTimer(&mockGame, tickInterval);

			// The timer job will change this to true when it executes
			bool jobExecuted = false;

			mockTimer.stop();

			mockTimer.insertJob(std::make_shared<MockTimerJob>(
				&mockGame, 1, 1, 0, [&]() {
					jobExecuted = true;
				}
			));

			mockTimer.start();
			std::this_thread::sleep_for(threadSleepTime);
			mockTimer.stop();

			CHECK(jobExecuted);
		}

		SUBCASE("Inserting job into started timer") {

			static std::chrono::milliseconds threadSleepTime(tickInterval * 2);

			trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());
			trogdor::Timer mockTimer(&mockGame, tickInterval);

			// The timer job will change this to true when it executes
			bool jobExecuted = false;

			mockTimer.start();

			mockTimer.insertJob(std::make_shared<MockTimerJob>(
				&mockGame, 1, 1, 0, [&]() {
					jobExecuted = true;
				}
			));

			std::this_thread::sleep_for(threadSleepTime);
			mockTimer.stop();

			CHECK(jobExecuted);
		}
	}

	TEST_CASE("Timer (timer/timer.cpp): Test for proper execution of jobs") {

		SUBCASE("Test interval") {

			// Number of clock ticks to wait between job executions
			int jobInterval = 5;

			static std::chrono::milliseconds threadSleepTime(tickInterval * (jobInterval + 1));

			trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());
			trogdor::Timer mockTimer(&mockGame, tickInterval);

			// Each time the job runs, it will increment this value
			int numExecutions = 0;

			mockTimer.insertJob(std::make_shared<MockTimerJob>(
				&mockGame, jobInterval, -1, 0, [&]() {
					numExecutions++;
				}
			));

			mockTimer.start();
			std::this_thread::sleep_for(threadSleepTime);
			mockTimer.stop();

			// Make sure the job only executed once during the defined interval
			CHECK(1 == numExecutions);
		}

		SUBCASE("Test # of executions") {

			int expectedExecutions = 3;
			static std::chrono::milliseconds threadSleepTime(tickInterval * expectedExecutions * 2);

			trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());
			trogdor::Timer mockTimer(&mockGame, tickInterval);

			// Each time the job runs, it will increment this value
			int numExecutions = 0;

			mockTimer.insertJob(std::make_shared<MockTimerJob>(
				&mockGame, 1, expectedExecutions, 0, [&]() {
					numExecutions++;
				}
			));

			mockTimer.start();
			std::this_thread::sleep_for(threadSleepTime);
			mockTimer.stop();

			CHECK(expectedExecutions == numExecutions);
		}

		SUBCASE("Test start time") {

			// Job should start after 3 clock ticks have passed after
			// insertion
			int startTime = 3;
			static std::chrono::milliseconds threadSleepTime(tickInterval * (startTime + 1));

			trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());
			trogdor::Timer mockTimer(&mockGame, tickInterval);

			// When the job runs, it will record the start time here
			int actualStartTime = 0;

			mockTimer.insertJob(std::make_shared<MockTimerJob>(
				&mockGame, 1, 1, startTime, [&]() {
					actualStartTime = mockTimer.getTime();
				}
			));

			mockTimer.start();
			std::this_thread::sleep_for(threadSleepTime);
			mockTimer.stop();

			// I know this is lame, but I have to allow for an off-by-one in
			// the positive direction because the timer isn't super accurate
			// and sometimes the timing of threads is a little weird. This
			// issue will hopefully disappear when/if I come up with a more
			// reliable way to handle time. Until then, I'm going to consider
			// either of these results to be correct. NOTE: I have to store
			// the result in a variable first because CHECK() will only
			// accept a binary comparison.
			bool result = actualStartTime == startTime || actualStartTime == startTime + 1;
			CHECK(result);
		}
	}

	TEST_CASE("Timer (timer/timer.cpp): removeJob()") {

		static std::chrono::milliseconds sleepTime(tickInterval * 2);

		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());
		trogdor::Timer mockTimer(&mockGame, tickInterval);

		// Each time the job runs, it will increment this value
		int numExecutions = 0;

		// Create a timer job with an unlimited number of executions
		std::shared_ptr<MockTimerJob> job = std::make_shared<MockTimerJob>(
			&mockGame, 1, -1, 0, [&]() {
				numExecutions++;
			}
		);

		mockTimer.insertJob(job);

		// Start and wait for the timer to run long enough to execute the job
		// once
		mockTimer.start();
		std::this_thread::sleep_for(sleepTime);

		// Remove the job and then wait some more time to ensure it no longer
		// executes
		mockTimer.removeJob(job);

		std::this_thread::sleep_for(sleepTime);
		mockTimer.stop();

		// I know this is lame, but I have to allow for an off-by-one in both
		// directions (theoretically, the job should execute twice, but could
		// excute one or three times) because of imprecision in the way I'm
		// administering this test, compounded by the fact that, due to the
		// way mutexes are handled, it's possible that a call to removeJob
		// will occur just as the timer is being incremented, resulting in
		// removeJob being blocked until another tick of the timer completes,
		// executing the job one more time. Note that I have to store the
		// result in a variable first, because CHECK() will only accept a
		// binary comparison.
		bool result = numExecutions >= 1 && numExecutions <= 3;
		CHECK(result);
	}
}
