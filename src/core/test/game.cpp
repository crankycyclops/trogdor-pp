#include <doctest.h>

#include <atomic>
#include <chrono>
#include <future>
#include <memory>
#include <thread>
#include <vector>

#include <trogdor/game.h>

#include <trogdor/entities/room.h>
#include <trogdor/entities/player.h>

#include <trogdor/iostream/nullout.h>
#include <trogdor/iostream/nullerr.h>

#include <trogdor/exception/entityexception.h>
#include <trogdor/exception/undefinedexception.h>


// Helper that constructs a standalone Room not yet attached to any game
static std::shared_ptr<trogdor::entity::Room> makeRoom(
	trogdor::Game *game,
	std::string name
) {

	return std::make_shared<trogdor::entity::Room>(
		game,
		name,
		std::make_unique<trogdor::NullOut>(),
		std::make_unique<trogdor::NullErr>()
	);
}


TEST_SUITE("Game (game.cpp)") {

	TEST_CASE("Game (game.cpp): insertEntity() inserts a room and the room is retrievable") {

		trogdor::Game game(std::make_unique<trogdor::NullErr>());

		game.insertEntity("r", makeRoom(&game, "r"));

		auto entity = game.getEntity("r");
		REQUIRE(entity != nullptr);
		CHECK(entity->getName() == "r");
	}

	TEST_CASE("Game (game.cpp): insertEntity() rejects duplicate names") {

		trogdor::Game game(std::make_unique<trogdor::NullErr>());

		game.insertEntity("r", makeRoom(&game, "r"));

		// A second insert under the same name must throw an exception
		CHECK_THROWS_AS(
			game.insertEntity("r", makeRoom(&game, "r")),
			trogdor::entity::EntityException
		);

		// The original entity should remain untouched
		CHECK(game.getEntity("r") != nullptr);
	}

	TEST_CASE("Game (game.cpp): insertEntity() releases the mutex when it throws rather than blocking indefinitely") {

		using namespace std::chrono_literals;

		trogdor::Game game(std::make_unique<trogdor::NullErr>());

		// Inserting a Player via insertEntity() throws UndefinedException and it
		// does so after the internal lock has been taken. Verify we don't
		// permanently lock when this occurs.
		auto player = std::make_shared<trogdor::entity::Player>(
			&game,
			"p",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		CHECK_THROWS_AS(
			game.insertEntity("p", player),
			trogdor::UndefinedException
		);

		// If the mutex has been leaked above, this insert on a different thread
		// will block forever. Assert with a generous timeout (not sure how else
		// to test that.)
		auto future = std::async(std::launch::async, [&game]() {
			game.insertEntity("r", makeRoom(&game, "r"));
		});

		REQUIRE(future.wait_for(5s) == std::future_status::ready);
		future.get(); // re-throw anything that escaped the worker thread

		CHECK(game.getEntity("r") != nullptr);
	}

	TEST_CASE("Game (game.cpp): insertEntity() check and insert logic is atomic under concurrent inserts") {

		constexpr int numThreads = 32;
		trogdor::Game game(std::make_unique<trogdor::NullErr>());

		// Preconstruct one Room per thread, all sharing the same contested name
		// so the only thing the threads race on is insertEntity() itself
		std::vector<std::shared_ptr<trogdor::entity::Room>> rooms;
		for (int i = 0; i < numThreads; i++) {
			rooms.push_back(makeRoom(&game, "contested"));
		}

		std::atomic<int> successes(0);
		std::atomic<bool> go(false);
		std::vector<std::thread> threads;

		for (int i = 0; i < numThreads; i++) {

			threads.emplace_back([&game, &rooms, &successes, &go, i]() {

				// Spin until released so all threads pile into insertEntity() at
				// roughly the same moment
				while (!go.load()) {}

				try {
					game.insertEntity("contested", rooms[i]);
					successes++;
				}

				catch (const trogdor::entity::EntityException &) {
					// Expected for every thread but the one that wins
				}
			});
		}

		go.store(true);

		for (auto &t: threads) {
			t.join();
		}

		// Exactly one insert should succeed
		CHECK(successes.load() == 1);
		CHECK(game.getEntity("contested") != nullptr);
	}

	TEST_CASE("Game (game.cpp): lock()/unlock() make Game a recursive BasicLockable") {

		trogdor::Game game(std::make_unique<trogdor::NullErr>());

		SUBCASE("Game can be used directly with std::lock_guard") {

			// Locking and unlocking via RAII should compile and run cleanly...
			{
				std::lock_guard<trogdor::Game> lock(game);
			}

			// ...and once the guard releases, another thread can take the lock.
			std::atomic<bool> acquired(false);

			std::thread t([&]() {
				std::lock_guard<trogdor::Game> lock(game);
				acquired = true;
			});

			t.join();
			CHECK(acquired.load());
		}

		SUBCASE("The same thread may lock recursively without deadlocking") {

			// The underlying mutex is recursive, so nested locks on a single
			// thread must not deadlock. Reaching the CHECK is the assertion.
			std::lock_guard<trogdor::Game> outer(game);
			std::lock_guard<trogdor::Game> inner(game);

			CHECK(true);
		}

		SUBCASE("A second thread is excluded while the lock is held") {

			std::atomic<bool> contenderGotLock(false);
			std::thread contender;

			{
				std::lock_guard<trogdor::Game> lock(game);

				contender = std::thread([&]() {
					std::lock_guard<trogdor::Game> l(game);
					contenderGotLock = true;
				});

				// Give the contender time to block on the lock, then confirm it
				// can't acquire the mutex while we still hold it.
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
				CHECK(!contenderGotLock.load());
			}

			// Releasing our guard should let the contender proceed.
			contender.join();
			CHECK(contenderGotLock.load());
		}
	}
}
