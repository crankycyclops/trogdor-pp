#include <doctest.h>
#include <trogdor/vocabulary.h>
#include <trogdor/exception/validationexception.h>

#include "mock/mockaction.h"


TEST_SUITE("Vocabulary (vocabulary.cpp)") {

	TEST_CASE("Vocabulary (vocabulary.cpp): Test for sane initialization and defaults") {

		trogdor::Vocabulary vocabulary;

		// These built-in directions should always be initialized during
		// construction.
		CHECK(vocabulary.isDirection("north"));
		CHECK(vocabulary.isDirection("south"));
		CHECK(vocabulary.isDirection("east"));
		CHECK(vocabulary.isDirection("west"));
		CHECK(vocabulary.isDirection("northeast"));
		CHECK(vocabulary.isDirection("northwest"));
		CHECK(vocabulary.isDirection("southeast"));
		CHECK(vocabulary.isDirection("southwest"));
		CHECK(vocabulary.isDirection("up"));
		CHECK(vocabulary.isDirection("down"));
		CHECK(vocabulary.isDirection("inside"));
		CHECK(vocabulary.isDirection("outside"));

		CHECK(vocabulary.isDirectionSynonym("n"));
		CHECK(vocabulary.isDirectionSynonym("s"));
		CHECK(vocabulary.isDirectionSynonym("e"));
		CHECK(vocabulary.isDirectionSynonym("w"));
		CHECK(vocabulary.isDirectionSynonym("ne"));
		CHECK(vocabulary.isDirectionSynonym("nw"));
		CHECK(vocabulary.isDirectionSynonym("se"));
		CHECK(vocabulary.isDirectionSynonym("sw"));
		CHECK(vocabulary.isDirectionSynonym("in"));
		CHECK(vocabulary.isDirectionSynonym("out"));

		// The following filler word should always be initialized.
		CHECK(vocabulary.isFillerWord("the"));
	}

	TEST_CASE("Vocabulary (vocabulary.cpp): insertDirection(), isDirection(), insertDirectionSynonym() and isDirectionSynonym()") {

		trogdor::Vocabulary vocabulary;

		CHECK(!vocabulary.isDirection("direction"));
		vocabulary.insertDirection("direction");
		CHECK(vocabulary.isDirection("direction"));

		// Make sure we can't insert a synonym for a non-existent direction
		try {
			vocabulary.insertDirectionSynonym("synonym", "nonexistentDirection");
			CHECK(false);
		}

		catch (const trogdor::ValidationException &e) {
			CHECK(true);
		}

		CHECK(!vocabulary.isDirectionSynonym("synonym"));
		vocabulary.insertDirectionSynonym("synonym", "direction");
		CHECK(vocabulary.isDirectionSynonym("synonym"));
	}

	TEST_CASE("Vocabulary (vocabulary.cpp): getDirection()") {

		trogdor::Vocabulary vocabulary;

		vocabulary.insertDirection("direction");
		vocabulary.insertDirectionSynonym("synonym", "direction");

		// Validate that we can return a direction by both its name and one of
		// its synonyms.
		CHECK(0 == vocabulary.getDirection("direction").compare("direction"));
		CHECK(0 == vocabulary.getDirection("synonym").compare("direction"));

		// Validate that we can't get a string that isn't a direction or a synonym
		CHECK(0 == vocabulary.getDirection("nonexistentDirection").compare(""));
	}

	TEST_CASE("Vocabulary (vocabulary.cpp): insertFillerWord() and isFillerWord()") {

		trogdor::Vocabulary vocabulary;

		CHECK(!vocabulary.isFillerWord("fillerword"));
		vocabulary.insertFillerWord("fillerword");
		CHECK(vocabulary.isFillerWord("fillerword"));
	}

	TEST_CASE("Vocabulary (vocabulary.cpp): insertPreposition() and isPreposition()") {

		trogdor::Vocabulary vocabulary;

		CHECK(!vocabulary.isPreposition("prep"));
		vocabulary.insertPreposition("prep");
		CHECK(vocabulary.isPreposition("prep"));
	}

	TEST_CASE("Vocabulary (vocabulary.cpp): Verb action and synonym methods") {

		trogdor::Vocabulary vocabulary;

		std::unique_ptr<MockAction> action = std::make_unique<MockAction>();
		MockAction *rawActionPtr = action.get();

		// Make sure we can insert verbs
		CHECK(!vocabulary.isVerb("verb"));
		vocabulary.insertVerbAction("verb", std::move(action));
		CHECK(vocabulary.isVerb("verb"));

		// Make sure we can't insert a synonym for a verb that doesn't exist
		try {
			vocabulary.insertVerbSynonym("synonym", "doesntExist");
			CHECK(false);
		}

		catch (const trogdor::ValidationException &e) {
			CHECK(true);
		}

		// Make sure isVerb() works for both verbs and synonyms
		vocabulary.insertVerbSynonym("synonym", "verb");
		CHECK(vocabulary.isVerb("synonym"));

		// Make sure isVerbSynonym() returns true for synonyms and false for
		// actual verbs
		CHECK(vocabulary.isVerbSynonym("synonym"));
		CHECK(!vocabulary.isVerbSynonym("verb"));

		// Make sure we can get actions from their verbs or verb synonyms
		CHECK(rawActionPtr == vocabulary.getVerbAction("verb"));
		CHECK(rawActionPtr == vocabulary.getVerbAction("synonym"));
		CHECK(!vocabulary.getVerbAction("doesntExist"));

		// Make sure we can completely remove verb actions and be in a sane state
		vocabulary.removeVerbAction("verb");
		CHECK(!vocabulary.getVerbAction("verb"));
		CHECK(!vocabulary.getVerbAction("synonym"));
		CHECK(!vocabulary.isVerb("verb"));
		CHECK(!vocabulary.isVerb("synonym"));
		CHECK(!vocabulary.isVerbSynonym("synonym"));

		// Make sure that if we add a verb and a synonym, remove the verb,
		// then add back the synonym, this time for the second verb, that
		// we end up with the proper synonym -> verb mapping.
		std::unique_ptr<MockAction> action2 = std::make_unique<MockAction>();
		MockAction *rawActionPtr2 = action2.get();

		vocabulary.insertVerbAction("verb2", std::move(action2));
		vocabulary.insertVerbSynonym("synonym", "verb2");
		CHECK(vocabulary.isVerbSynonym("synonym"));
		CHECK(vocabulary.isVerb("synonym"));
		CHECK(rawActionPtr2 == vocabulary.getVerbAction("verb2"));
		CHECK(rawActionPtr2 == vocabulary.getVerbAction("synonym"));
	}
}
