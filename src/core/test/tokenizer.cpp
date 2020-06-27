#include <doctest.h>
#include <trogdor/tokenizer.h>


TEST_SUITE("Tokenizer (tokenizer.cpp)") {

	TEST_CASE("Tokenizer (tokenizer.cpp): Empty string") {

		trogdor::Tokenizer empty("");

		// Make sure the tokenizer doesn't choke on empty strings
		CHECK(0 == empty.size());
		CHECK(empty.isEnd());
		CHECK(0 == empty.getCurToken().compare(""));
		CHECK(0 == empty.consumeAll().size());
	}

	TEST_CASE("Tokenizer (tokenizer.cpp): Whitespace-only string") {

		trogdor::Tokenizer white(" \t ");

		// Make sure the tokenizer doesn't choke on whitespace-only strings,
		// which like empty strings, contain no tokens
		CHECK(0 == white.size());
		CHECK(white.isEnd());
		CHECK(0 == white.getCurToken().compare(""));
		CHECK(0 == white.consumeAll().size());
	}

	TEST_CASE("Tokenizer (tokenizer.cpp): \"This is a test.\"") {

		SUBCASE("Test for sane initialization (also tests size())") {

			trogdor::Tokenizer tokens("This is a test.");

			CHECK(4 == tokens.size());
			CHECK(!tokens.isEnd());
			CHECK(0 == tokens.getCurToken().compare("This"));
		}

		SUBCASE("Test consumeAll()") {

			trogdor::Tokenizer tokens("This is a test.");

			auto allTokens = tokens.consumeAll();

			// consumeAll() should advance the tokenizer all the way to the end
			CHECK(tokens.isEnd());

			CHECK(4 == allTokens.size());
			CHECK(0 == allTokens[0].compare("This"));
			CHECK(0 == allTokens[1].compare("is"));
			CHECK(0 == allTokens[2].compare("a"));
			CHECK(0 == allTokens[3].compare("test."));
		}

		SUBCASE("Test rewind()") {

			trogdor::Tokenizer tokens("This is a test.");

			tokens.consumeAll();
			tokens.rewind();

			CHECK(!tokens.isEnd());
			CHECK(0 == tokens.getCurToken().compare("This"));
		}

		SUBCASE("Test next()") {

			trogdor::Tokenizer tokens("This is a test.");

			tokens.next();
			CHECK(!tokens.isEnd());
			CHECK(0 == tokens.getCurToken().compare("is"));

			tokens.next();
			CHECK(!tokens.isEnd());
			CHECK(0 == tokens.getCurToken().compare("a"));

			tokens.next();
			CHECK(!tokens.isEnd());
			CHECK(0 == tokens.getCurToken().compare("test."));

			// Verify that we can reach the end and be able to test when to
			// stop looking for more tokens.
			tokens.next();
			CHECK(tokens.isEnd());
			CHECK(0 == tokens.getCurToken().compare(""));

			// Verify that continuing to call next() after we've reached the
			// end doesn't cause any weird undefined behavior.
			tokens.next();
			CHECK(tokens.isEnd());
			CHECK(0 == tokens.getCurToken().compare(""));
		}

		SUBCASE("Test previous()") {

			trogdor::Tokenizer tokens("This is a test.");

			tokens.consumeAll();

			tokens.previous();
			CHECK(!tokens.isEnd());
			CHECK(0 == tokens.getCurToken().compare("test."));

			tokens.previous();
			CHECK(!tokens.isEnd());
			CHECK(0 == tokens.getCurToken().compare("a"));

			tokens.previous();
			CHECK(!tokens.isEnd());
			CHECK(0 == tokens.getCurToken().compare("is"));

			tokens.previous();
			CHECK(!tokens.isEnd());
			CHECK(0 == tokens.getCurToken().compare("This"));

			// Make sure calling previous() when already at the beginning
			// doesn't result in undefined behavior.
			tokens.previous();
			CHECK(!tokens.isEnd());
			CHECK(0 == tokens.getCurToken().compare("This"));
		}
	}
}
