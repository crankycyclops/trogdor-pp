#include <doctest.h>
#include "helper.h"


// Test game name
static const char *gameName = "myGame";

TEST_SUITE("EntityController (scopes/entity.cpp)") {

	TEST_CASE("EntityController (scopes/entity.cpp): getEntity()") {

		SUBCASE("No args") {
			// TODO
		}

		SUBCASE("Empty args") {
			// TODO
		}

		SUBCASE("Missing game id") {
			// TODO
		}

		SUBCASE("Missing entity name") {
			// TODO
		}

		SUBCASE("No games, invalid game id") {
			// TODO
		}

		SUBCASE("One game, invalid game id") {
			// TODO
		}

		SUBCASE("One game, invalid entity name") {
			// TODO
		}

		SUBCASE("Stopped game, valid game id and entity name") {
			// TODO
		}

		SUBCASE("Started game, valid game id and entity name") {
			// TODO
		}
	}

	TEST_CASE("EntityController (scopes/entity.cpp): getEntityList()") {

		SUBCASE("No args") {
			// TODO
		}

		SUBCASE("Empty args") {
			// TODO
		}

		SUBCASE("Missing game id") {
			// TODO
		}

		SUBCASE("No games, invalid game id") {
			// TODO
		}

		SUBCASE("One game, invalid game id") {
			// TODO
		}

		SUBCASE("Stopped game, valid game id") {
			// TODO
		}

		SUBCASE("Started game, valid game id") {
			// TODO
		}
	}

	// For now, I'm only testing the default output driver. Later, I'll want
	// to alter the config to test the redis controller, which would return an
	// operation not supported error that I'd need to also validate.
	TEST_CASE("EntityController (scopes/entity.cpp): getOutput()") {

		SUBCASE("No args") {
			// TODO
		}

		SUBCASE("Empty args") {
			// TODO
		}

		SUBCASE("Missing game id") {
			// TODO
		}

		SUBCASE("Missing entity name") {
			// TODO
		}

		SUBCASE("Missing channel") {
			// TODO
		}

		SUBCASE("No games, invalid game id") {
			// TODO
		}

		SUBCASE("One game, invalid game id") {
			// TODO
		}

		SUBCASE("No games, invalid entity name") {
			// TODO
		}

		SUBCASE("One game, invalid entity name") {
			// TODO
		}

		SUBCASE("Invalid channel (any non-string type)") {
			// TODO: must be a string (test scalers like numerics and bools as well as non-scalars like objects and arrays)
		}

		SUBCASE("Stopped game, valid game id, entity name, and channel") {
			// TODO: test both channels that are used which will return non-empty
			// arrays of messages and channels that aren't used that will return
			// an empty array.
		}

		SUBCASE("Started game, valid game id, entity name, and channel") {
			// TODO: test both channels that are used which will return non-empty
			// arrays of messages and channels that aren't used that will return
			// an empty array.
		}
	}

	TEST_CASE("EntityController (scopes/entity.cpp): appendOutput()") {

		SUBCASE("No args") {
			// TODO
		}

		SUBCASE("Empty args") {
			// TODO
		}

		SUBCASE("Missing game id") {
			// TODO
		}

		SUBCASE("Missing entity name") {
			// TODO
		}

		SUBCASE("Missing channel") {
			// TODO
		}

		SUBCASE("No games, invalid game id") {
			// TODO
		}

		SUBCASE("One game, invalid game id") {
			// TODO
		}

		SUBCASE("No games, invalid entity name") {
			// TODO
		}

		SUBCASE("One game, invalid entity name") {
			// TODO
		}

		SUBCASE("Invalid channel (any non-string type)") {
			// TODO: must be a string (test scalers like numerics and bools as well as non-scalars like objects and arrays)
		}

		SUBCASE("Invalid message (any non-string type)") {
			// TODO: must be a string (test scalers like numerics and bools as well as non-scalars like objects and arrays)
		}

		SUBCASE("Stopped game, valid game id, entity name, channel, and message") {
			// TODO
		}

		SUBCASE("Started game, valid game id, entity name, channel, and message") {
			// TODO
		}
	}
}
