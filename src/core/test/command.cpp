#include <doctest.h>
#include <trogdor/game.h>
#include <trogdor/command.h>
#include <trogdor/iostream/nullout.h>
#include <trogdor/iostream/nullerr.h>
#include <trogdor/entities/player.h>


TEST_SUITE("Command (command.cpp)") {

	TEST_CASE("Command (command.cpp): getVocabulary()") {

		trogdor::Vocabulary vocabulary;
		trogdor::Command command(vocabulary, "test");

		CHECK(&vocabulary == &command.getVocabulary());
	}

	TEST_CASE("Command (command.cpp): Test null input (without whitespace)") {

		trogdor::Vocabulary mockVocab;
		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());
		trogdor::Command command(mockVocab, "");

		CHECK(!command.isInvalid());
		CHECK(command.isNull());
	}

	TEST_CASE("Command (command.cpp): Test null input (with whitespace)") {

		trogdor::Vocabulary mockVocab;
		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());
		trogdor::Command command(mockVocab, " \t \n   ");

		CHECK(!command.isInvalid());
		CHECK(command.isNull());
	}

	TEST_CASE("Command (command.cpp): Test parsing of single word sentences with valid verb") {

		trogdor::Vocabulary mockVocab;
		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());
		trogdor::Command command(mockVocab, "north");

		CHECK(!command.isInvalid());
		CHECK(!command.isNull());
		CHECK(0 == command.getVerb().compare("north"));
		CHECK(0 == command.getDirectObject().compare(""));
		CHECK(0 == command.getIndirectObject().compare(""));
		CHECK(0 == command.getPreposition().compare(""));
	}

	TEST_CASE("Command (command.cpp): Test parsing of verb + single word direct object") {

		trogdor::Vocabulary mockVocab;
		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());
		trogdor::Command command(mockVocab, "take candle");

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
		trogdor::Command command(mockVocab, "take the candle");

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
		trogdor::Command command(mockVocab, "take damp candle holder");

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
		trogdor::Command command(mockVocab, "take the damp candle holder");

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
		trogdor::Command command(mockVocab, "take candle from wizard");

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
		trogdor::Command command(mockVocab, "take wet candle from stupid wizard");

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
		trogdor::Command command(mockVocab, "take the wet candle from the stupid wizard");

		CHECK(!command.isInvalid());
		CHECK(!command.isNull());
		CHECK(0 == command.getVerb().compare("take"));
		CHECK(0 == command.getDirectObject().compare("wet candle"));
		CHECK(0 == command.getIndirectObject().compare("stupid wizard"));
		CHECK(0 == command.getPreposition().compare("from"));
	}

	TEST_CASE("Command (command.cpp): Invalid case #1: single word sentence with unrecognized verb") {

		trogdor::Vocabulary mockVocab;
		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());
		trogdor::Command command(mockVocab, "give");

		CHECK(command.isInvalid());
		CHECK(!command.isNull());
		CHECK(0 == command.getVerb().compare(""));
		CHECK(0 == command.getDirectObject().compare(""));
		CHECK(0 == command.getIndirectObject().compare(""));
		CHECK(0 == command.getPreposition().compare(""));
	}

	TEST_CASE("Command (command.cpp): Invalid case #2: verb + do + prep + ido with unrecognized verb") {

		trogdor::Vocabulary mockVocab;
		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());
		trogdor::Command command(mockVocab, "notaverb candle to wizard");

		CHECK(command.isInvalid());
		CHECK(!command.isNull());
		CHECK(0 == command.getVerb().compare(""));
		CHECK(0 == command.getDirectObject().compare(""));
		CHECK(0 == command.getIndirectObject().compare(""));
		CHECK(0 == command.getPreposition().compare(""));
	}

	TEST_CASE("Command (command.cpp): Invalid case #3: dangling preposition") {

		trogdor::Vocabulary mockVocab;
		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());
		trogdor::Command command(mockVocab, "take the wet candle from");

		CHECK(command.isInvalid());
		CHECK(!command.isNull());
		CHECK(0 == command.getVerb().compare("take"));
		CHECK(0 == command.getDirectObject().compare("wet candle"));
		CHECK(0 == command.getIndirectObject().compare(""));
		CHECK(0 == command.getPreposition().compare("from"));
	}

	TEST_CASE("Command (command.cpp): Invalid case #4: verb + dangling filler") {

		trogdor::Vocabulary mockVocab;
		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());
		trogdor::Command command(mockVocab, "take the");

		CHECK(command.isInvalid());
		CHECK(!command.isNull());
		CHECK(0 == command.getVerb().compare("take"));
		CHECK(0 == command.getDirectObject().compare(""));
		CHECK(0 == command.getIndirectObject().compare(""));
		CHECK(0 == command.getPreposition().compare(""));
	}
}