#include <doctest.h>
#include <trogdor/game.h>
#include <trogdor/command.h>
#include <trogdor/iostream/nullout.h>
#include <trogdor/iostream/nullerr.h>
#include <trogdor/entities/player.h>

#include "mock/stringin.h"


TEST_SUITE("Command (command.cpp)") {

	TEST_CASE("Command (command.cpp): Test for sane initialization") {

		trogdor::Vocabulary vocabulary;
		trogdor::Command command(vocabulary);

		CHECK(command.isInvalid());
	}

	TEST_CASE("Command (command.cpp): getVocabulary()") {

		trogdor::Vocabulary vocabulary;
		trogdor::Command command(vocabulary);

		CHECK(&vocabulary == &command.getVocabulary());
	}

	TEST_CASE("Command (command.cpp): Test null input (without whitespace)") {

		trogdor::Vocabulary mockVocab;
		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());
		trogdor::Command command(mockVocab);

		trogdor::entity::Player mockPlayer(
			&mockGame,
			"player",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<StringIn>(""),
			std::make_unique<trogdor::NullErr>()
		);

		command.read(&mockPlayer);

		CHECK(!command.isInvalid());
		CHECK(command.isNull());
	}

	TEST_CASE("Command (command.cpp): Test null input (with whitespace)") {

		trogdor::Vocabulary mockVocab;
		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());
		trogdor::Command command(mockVocab);

		trogdor::entity::Player mockPlayer(
			&mockGame,
			"player",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<StringIn>(" \t \n   "),
			std::make_unique<trogdor::NullErr>()
		);

		command.read(&mockPlayer);

		CHECK(!command.isInvalid());
		CHECK(command.isNull());
	}

	TEST_CASE("Command (command.cpp): Test parsing of single word sentences with valid verb") {

		trogdor::Vocabulary mockVocab;
		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());
		trogdor::Command command(mockVocab);

		trogdor::entity::Player mockPlayer(
			&mockGame,
			"player",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<StringIn>("north"),
			std::make_unique<trogdor::NullErr>()
		);

		command.read(&mockPlayer);

		CHECK(!command.isInvalid());
		CHECK(!command.isNull());
		CHECK(0 == command.getVerb().compare("north"));
		CHECK(0 == command.getDirectObject().compare(""));
		CHECK(0 == command.getIndirectObject().compare(""));
		CHECK(0 == command.getPreposition().compare(""));
	}

	TEST_CASE("Command (command.cpp): Test parsing of verb + do + prep + ido with invalid verb") {

		trogdor::Vocabulary mockVocab;
		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());
		trogdor::Command command(mockVocab);

		// The verb "give" isn't curr
		trogdor::entity::Player mockPlayer(
			&mockGame,
			"player",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<StringIn>("notaverb candle to wizard"),
			std::make_unique<trogdor::NullErr>()
		);

		command.read(&mockPlayer);

		CHECK(command.isInvalid());
		CHECK(!command.isNull());
		CHECK(0 == command.getVerb().compare(""));
		CHECK(0 == command.getDirectObject().compare(""));
		CHECK(0 == command.getIndirectObject().compare(""));
		CHECK(0 == command.getPreposition().compare(""));
	}

	TEST_CASE("Command (command.cpp): Test parsing of verb + single word direct object") {

		trogdor::Vocabulary mockVocab;
		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());
		trogdor::Command command(mockVocab);

		trogdor::entity::Player mockPlayer(
			&mockGame,
			"player",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<StringIn>("take candle"),
			std::make_unique<trogdor::NullErr>()
		);

		command.read(&mockPlayer);

		CHECK(!command.isInvalid());
		CHECK(!command.isNull());
		CHECK(0 == command.getVerb().compare("take"));
		CHECK(0 == command.getDirectObject().compare("candle"));
		CHECK(0 == command.getIndirectObject().compare(""));
		CHECK(0 == command.getPreposition().compare(""));
	}

	TEST_CASE("Command (command.cpp): Test parsing of verb + filler word + single word direct object") {

		trogdor::Vocabulary mockVocab;
		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());
		trogdor::Command command(mockVocab);

		trogdor::entity::Player mockPlayer(
			&mockGame,
			"player",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<StringIn>("take the candle"),
			std::make_unique<trogdor::NullErr>()
		);

		command.read(&mockPlayer);

		CHECK(!command.isInvalid());
		CHECK(!command.isNull());
		CHECK(0 == command.getVerb().compare("take"));
		CHECK(0 == command.getDirectObject().compare("candle"));
		CHECK(0 == command.getIndirectObject().compare(""));
		CHECK(0 == command.getPreposition().compare(""));
	}

	TEST_CASE("Command (command.cpp): Test parsing of verb + multi-word direct object") {

		trogdor::Vocabulary mockVocab;
		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());
		trogdor::Command command(mockVocab);

		trogdor::entity::Player mockPlayer(
			&mockGame,
			"player",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<StringIn>("take damp candle holder"),
			std::make_unique<trogdor::NullErr>()
		);

		command.read(&mockPlayer);

		CHECK(!command.isInvalid());
		CHECK(!command.isNull());
		CHECK(0 == command.getVerb().compare("take"));
		CHECK(0 == command.getDirectObject().compare("damp candle holder"));
		CHECK(0 == command.getIndirectObject().compare(""));
		CHECK(0 == command.getPreposition().compare(""));
	}

	TEST_CASE("Command (command.cpp): Test parsing of verb + filler word + multi-word direct object") {

		trogdor::Vocabulary mockVocab;
		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());
		trogdor::Command command(mockVocab);

		trogdor::entity::Player mockPlayer(
			&mockGame,
			"player",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<StringIn>("take the damp candle holder"),
			std::make_unique<trogdor::NullErr>()
		);

		command.read(&mockPlayer);

		CHECK(!command.isInvalid());
		CHECK(!command.isNull());
		CHECK(0 == command.getVerb().compare("take"));
		CHECK(0 == command.getDirectObject().compare("damp candle holder"));
		CHECK(0 == command.getIndirectObject().compare(""));
		CHECK(0 == command.getPreposition().compare(""));
	}

	TEST_CASE("Command (command.cpp): Test parsing of verb + do + prep + ido") {

		trogdor::Vocabulary mockVocab;
		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());
		trogdor::Command command(mockVocab);

		// The verb "give" isn't curr
		trogdor::entity::Player mockPlayer(
			&mockGame,
			"player",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<StringIn>("take candle from wizard"),
			std::make_unique<trogdor::NullErr>()
		);

		command.read(&mockPlayer);

		CHECK(!command.isInvalid());
		CHECK(!command.isNull());
		CHECK(0 == command.getVerb().compare("take"));
		CHECK(0 == command.getDirectObject().compare("candle"));
		CHECK(0 == command.getIndirectObject().compare("wizard"));
		CHECK(0 == command.getPreposition().compare("from"));
	}

	TEST_CASE("Command (command.cpp): Test parsing of verb + multi-word do + prep + multi-word ido") {

		trogdor::Vocabulary mockVocab;
		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());
		trogdor::Command command(mockVocab);

		// The verb "give" isn't curr
		trogdor::entity::Player mockPlayer(
			&mockGame,
			"player",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<StringIn>("take wet candle from stupid wizard"),
			std::make_unique<trogdor::NullErr>()
		);

		command.read(&mockPlayer);

		CHECK(!command.isInvalid());
		CHECK(!command.isNull());
		CHECK(0 == command.getVerb().compare("take"));
		CHECK(0 == command.getDirectObject().compare("wet candle"));
		CHECK(0 == command.getIndirectObject().compare("stupid wizard"));
		CHECK(0 == command.getPreposition().compare("from"));
	}

	TEST_CASE("Command (command.cpp): Test parsing of verb + filler + multi-word do + prep + filler + multi-word ido") {

		trogdor::Vocabulary mockVocab;
		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());
		trogdor::Command command(mockVocab);

		// The verb "give" isn't curr
		trogdor::entity::Player mockPlayer(
			&mockGame,
			"player",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<StringIn>("take the wet candle from the stupid wizard"),
			std::make_unique<trogdor::NullErr>()
		);

		command.read(&mockPlayer);

		CHECK(!command.isInvalid());
		CHECK(!command.isNull());
		CHECK(0 == command.getVerb().compare("take"));
		CHECK(0 == command.getDirectObject().compare("wet candle"));
		CHECK(0 == command.getIndirectObject().compare("stupid wizard"));
		CHECK(0 == command.getPreposition().compare("from"));
	}

	TEST_CASE("Command (command.cpp): Invalid case #1: unrecognized verb") {

		trogdor::Vocabulary mockVocab;
		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());
		trogdor::Command command(mockVocab);

		trogdor::entity::Player mockPlayer(
			&mockGame,
			"player",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<StringIn>("give"),
			std::make_unique<trogdor::NullErr>()
		);

		command.read(&mockPlayer);

		CHECK(command.isInvalid());
		CHECK(!command.isNull());
		CHECK(0 == command.getVerb().compare(""));
		CHECK(0 == command.getDirectObject().compare(""));
		CHECK(0 == command.getIndirectObject().compare(""));
		CHECK(0 == command.getPreposition().compare(""));
	}

	TEST_CASE("Command (command.cpp): Invalid case #2: dangling preposition") {

		trogdor::Vocabulary mockVocab;
		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());
		trogdor::Command command(mockVocab);

		// The verb "give" isn't curr
		trogdor::entity::Player mockPlayer(
			&mockGame,
			"player",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<StringIn>("take the wet candle from"),
			std::make_unique<trogdor::NullErr>()
		);

		command.read(&mockPlayer);

		CHECK(command.isInvalid());
		CHECK(!command.isNull());
		CHECK(0 == command.getVerb().compare("take"));
		CHECK(0 == command.getDirectObject().compare("wet candle"));
		CHECK(0 == command.getIndirectObject().compare(""));
		CHECK(0 == command.getPreposition().compare("from"));
	}

	TEST_CASE("Command (command.cpp): Invalid case #3: verb + dangling filler") {

		trogdor::Vocabulary mockVocab;
		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());
		trogdor::Command command(mockVocab);

		trogdor::entity::Player mockPlayer(
			&mockGame,
			"player",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<StringIn>("take the"),
			std::make_unique<trogdor::NullErr>()
		);

		command.read(&mockPlayer);

		CHECK(command.isInvalid());
		CHECK(!command.isNull());
		CHECK(0 == command.getVerb().compare("take"));
		CHECK(0 == command.getDirectObject().compare(""));
		CHECK(0 == command.getIndirectObject().compare(""));
		CHECK(0 == command.getPreposition().compare(""));
	}
}
