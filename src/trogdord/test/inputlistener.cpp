#include <doctest.h>

#include <trogdor/game.h>
#include <trogdor/parser/parsers/xmlparser.h>
#include <trogdor/iostream/nullerr.h>
#include <trogdor/iostream/nullout.h>

#include "../include/io/input/driver.h"
#include "../include/io/iostream/serverin.h"

#include "../include/inputlistener.h"
#include "../include/io/iostream/serverin.h"

#include "mock/game/mockaction.h"


// Creates a test game for the purpose of unit testing.
static std::unique_ptr<trogdor::Game> makeGame(std::string definition) {

	std::unique_ptr<trogdor::Game> game = std::make_unique<trogdor::Game>(
		std::make_unique<trogdor::NullErr>()
	);

	std::unique_ptr<trogdor::XMLParser> parser = std::make_unique<trogdor::XMLParser>(
		game->makeInstantiator(),
		game->getVocabulary()
	);

	game->initialize(parser.get(), definition);

	return game;
}

/*****************************************************************************/

// Creates a test player inside the specified game for the purpose of unit testing.
// The mock game id works for now because, even though games are identified by the
// the input driver by id, that class itself doesn't actually rely on those ids
// to correspond to anything in GameContainer (the two classes aren't coupled.)
// If this ever chances, I'll have to rethink how this test is performed. One
// idea I had was to identify the game in the input driver by the ptr instead of
// the id. I'll think more about this if I ever make changes that break this
// functionality.
static std::shared_ptr<trogdor::entity::Player> makePlayer(trogdor::Game *game, size_t mockGameId, std::string name) {

	std::unique_ptr<ServerIn> inStream = std::make_unique<ServerIn>(mockGameId);
	ServerIn *rawInStreamPtr = inStream.get();

	std::shared_ptr<trogdor::entity::Player> player = game->createPlayer(
		name,
		std::make_unique<trogdor::NullOut>(),
		std::move(inStream),
		std::make_unique<trogdor::NullErr>()
	);

	rawInStreamPtr->setEntity(player.get());
	game->insertPlayer(player);
	return player;
}

/*****************************************************************************/

// Generates unique game ids
static size_t getNextGameId() {

	static size_t gameId = 0;
	return gameId++;
}

/*****************************************************************************/

TEST_SUITE("InputListener (inputlistener.cpp") {

	// Right now, all I can really check for is that no exceptions are thrown
	// when the instance is created.
	TEST_CASE("InputListener (inputlistener.cpp): Constructor (sane initialization)") {

		#ifndef CORE_UNIT_TEST_DEFINITION_FILE

			FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

		#else

			SUBCASE("Game started") {

				std::unique_ptr<trogdor::Game> game = makeGame(CORE_UNIT_TEST_DEFINITION_FILE);
				game->start();
				InputListener listener(game.get());

				CHECK(true);
			}

			SUBCASE("Game stopped") {

				std::unique_ptr<trogdor::Game> game = makeGame(CORE_UNIT_TEST_DEFINITION_FILE);
				game->stop();
				InputListener listener(game.get());

				CHECK(true);
			}

		#endif
	}

	TEST_CASE("InputListener (inputlistener.cpp): subscribe(), unsubscribe(), start(), and stop(); listener started") {

		#ifndef CORE_UNIT_TEST_DEFINITION_FILE

			FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

		#else

			SUBCASE("Game started") {

				size_t mockGameId = getNextGameId();

				// Incremented every time MockAction is executed
				int numExecutions = 0;

				std::unique_ptr<trogdor::Game> game = makeGame(CORE_UNIT_TEST_DEFINITION_FILE);
				std::shared_ptr<trogdor::entity::Player> player = makePlayer(game.get(), mockGameId, "player");

				InputListener listener(game.get());

				game->insertVerbAction("test", std::make_unique<MockAction>([&] {
					numExecutions++;
				}));

				game->start();

				// Send the "test" command to the player's input stream
				//input::Driver::get()->set(mockGameId, "player", "test");

				// TODO
				CHECK(true);
			}

			SUBCASE("Game stopped") {

				size_t mockGameId = getNextGameId();

				std::unique_ptr<trogdor::Game> game = makeGame(CORE_UNIT_TEST_DEFINITION_FILE);
				std::shared_ptr<trogdor::entity::Player> player = makePlayer(game.get(), mockGameId, "player");

				InputListener listener(game.get());

				game->stop();

				// TODO
				CHECK(true);
			}

		#endif
	}

	TEST_CASE("InputListener (inputlistener.cpp): subscribe(), unsubscribe(), start(), and stop(); listener stopped") {

		#ifndef CORE_UNIT_TEST_DEFINITION_FILE

			FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

		#else

			SUBCASE("Game started") {

				// TODO
			}

			SUBCASE("Game stopped") {

				// TODO
			}

		#endif
	}

	TEST_CASE("InputListener (inputlistener.cpp): subscribe(), unsubscribe(), start(), and stop(); listener stopped during subscribe(), then started before unsubscribe()") {

		#ifndef CORE_UNIT_TEST_DEFINITION_FILE

			FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

		#else

			SUBCASE("Game started") {

				// TODO
			}

			SUBCASE("Game stopped") {

				// TODO
			}

		#endif
	}

	TEST_CASE("InputListener (inputlistener.cpp): subscribe(), unsubscribe(), start(), and stop(); listener started during subscribe(), then stopped before unsubscribe(), then started again") {

		#ifndef CORE_UNIT_TEST_DEFINITION_FILE

			FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

		#else

			SUBCASE("Game started") {

				// TODO
			}

			SUBCASE("Game stopped") {

				// TODO
			}

		#endif
	}

	TEST_CASE("InputListener (inputlistener.cpp): Destructor, listener started") {

		#ifndef CORE_UNIT_TEST_DEFINITION_FILE

			FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

		#else

			SUBCASE("Game started") {

				// TODO
			}

			SUBCASE("Game stopped") {

				// TODO
			}

		#endif
	}

	TEST_CASE("InputListener (inputlistener.cpp): Destructor, listener stopped") {

		#ifndef CORE_UNIT_TEST_DEFINITION_FILE

			FAIL("CORE_UNIT_TEST_DEFINITION_FILE must be defined.");

		#else

			SUBCASE("Game started") {

				// TODO
			}

			SUBCASE("Game stopped") {

				// TODO
			}

		#endif
	}
}
