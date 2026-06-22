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

#include <trogdor/serial/serializable.h>

#include <trogdor/actions/action.h>
#include <trogdor/command.h>

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

// A minimal Action that records every player name it's asked to purge. Used to
// verify that Game::removePlayer propagates a purge to the registered actions
// (so player-specific action state can't accumulate after a player leaves.)
class PurgeRecordingAction: public trogdor::Action {

	private:

		std::vector<std::string> *purged;

	public:

		explicit PurgeRecordingAction(std::vector<std::string> *purgedNames):
			purged(purgedNames) {}

		virtual bool checkSyntax(const trogdor::Command &command) {return true;}

		virtual void execute(
			trogdor::entity::Player *player,
			const trogdor::Command &command,
			trogdor::Game *game
		) {}

		virtual void purgePlayer(const std::string &name) {

			purged->push_back(name);
		}
};


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

	TEST_CASE("Game (game.cpp): deserializing a malformed save throws a catchable exception instead of crashing") {

		// Stream factories for the deserializing Game constructor. The malformed
		// data below trips an exception before these are ever called, but the
		// constructor requires them.
		auto makeOut = [](trogdor::Game *) -> std::unique_ptr<trogdor::Trogout> {
			return std::make_unique<trogdor::NullOut>();
		};

		auto makeErr = [](trogdor::Game *) -> std::unique_ptr<trogdor::Trogerr> {
			return std::make_unique<trogdor::NullErr>();
		};

		SUBCASE("A missing key throws UndefinedException rather than dereferencing a disengaged optional") {

			// No "inGame" key at all. Previously, this dereferenced a disengaged
			// std::optional (undefined behavior). Now it must throw.
			auto data = std::make_shared<trogdor::serial::Serializable>();

			CHECK_THROWS_AS(
				trogdor::Game(data, std::make_unique<trogdor::NullErr>(), makeOut, makeErr),
				trogdor::UndefinedException
			);
		}

		SUBCASE("A wrong typed key throws UndefinedException rather than std::bad_variant_access") {

			// "inGame" is present and valid, but "introduction" is a string where
			// a nested object is expected.
			auto data = std::make_shared<trogdor::serial::Serializable>();
			data->set("inGame", false);
			data->set("introduction", "not an object");

			CHECK_THROWS_AS(
				trogdor::Game(data, std::make_unique<trogdor::NullErr>(), makeOut, makeErr),
				trogdor::UndefinedException
			);
		}

		SUBCASE("An entity with an empty \"types\" array throws UndefinedException rather than calling .back() on an empty vector") {

			// Build a valid save file by serializing a real game with one
			// entity, then corrupt only that entity's "types" to an empty array.
			// getValue() already covers the missing key and wrong type cases.
			// An empty array is the remaining vector that previously made the
			// .back() in Game::_deserialize undefined behavior.
			trogdor::Game source(std::make_unique<trogdor::NullErr>());
			source.insertEntity("r", makeRoom(&source, "r"));

			auto data = source.serialize();

			// The shared_ptrs inside the returned vector alias the Serializables
			// still held by data, so mutating through them corrupts the blob in
			// place.
			auto entities = data->getValue<
				std::vector<std::shared_ptr<trogdor::serial::Serializable>>
			>("entities");

			REQUIRE(entities.size() == 1);
			entities[0]->set("types", std::vector<std::string>{});

			CHECK_THROWS_AS(
				trogdor::Game(data, std::make_unique<trogdor::NullErr>(), makeOut, makeErr),
				trogdor::UndefinedException
			);
		}
	}

	TEST_CASE("Game (game.cpp): removePlayer() purges player-specific state from registered actions") {

		trogdor::Game game(std::make_unique<trogdor::NullErr>());

		// insertPlayer() places new players in the "start" room
		game.insertEntity("start", makeRoom(&game, "start"));

		// Register an action that records which players it's asked to purge
		std::vector<std::string> purged;
		game.insertVerbAction(
			"recordpurge",
			std::make_unique<PurgeRecordingAction>(&purged)
		);

		auto player = game.createPlayer(
			"alice",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);
		game.insertPlayer(player);

		// Removing the player must propagate a purge for that player's name to
		// the registered actions, so mid-clarification state (keyed by player
		// name) can't accumulate after the player has left the game.
		game.removePlayer("alice");

		REQUIRE(purged.size() == 1);
		CHECK(purged[0] == "alice");
	}

	TEST_CASE("Game (game.cpp): removePlayer() does nothing for a player that doesn't exist") {

		trogdor::Game game(std::make_unique<trogdor::NullErr>());

		std::vector<std::string> purged;
		game.insertVerbAction(
			"recordpurge",
			std::make_unique<PurgeRecordingAction>(&purged)
		);

		// No such player: removePlayer() should do nothing and must not purge
		game.removePlayer("nobody");

		CHECK(purged.empty());
	}
}
