#include <sstream>
#include <doctest.h>
#include <trogdor/messages.h>

// Test copy construction and assignment operator after testing get and set
TEST_SUITE("Messages (messages.cpp)") {

	TEST_CASE("Messages (messages.cpp): get(), set(), and clear()") {

		trogdor::Messages msgs;

		// Verify that unset/nonexistent messages return an empty string
		CHECK(0 == msgs.get("key").compare(""));

		// Verify that setting a value results in it being gettable
		msgs.set("key", "message");
		CHECK(0 == msgs.get("key").compare("message"));

		// Verify that setting a value a second time overwrites it
		msgs.set("key", "message2");
		CHECK(0 == msgs.get("key").compare("message2"));

		// Verify that calling clear removes set messages
		msgs.clear();
		CHECK(0 == msgs.get("key").compare(""));
	}

	TEST_CASE("Messages (messages.cpp): display()") {

		trogdor::Messages msgs;
		std::stringstream sStream;

		msgs.set("key", "message");
		msgs.display("key", sStream);

		// Note that display adds a newline at the end
		CHECK(0 == sStream.str().compare("message\n"));
	}

	TEST_CASE("Messages (messages.cpp): Copy constructor") {

		trogdor::Messages msgs1;
		msgs1.set("key", "message");

		trogdor::Messages msgs2(msgs1);
		CHECK(0 == msgs2.get("key").compare("message"));
	}

	TEST_CASE("Messages (messages.cpp): operator=") {

		trogdor::Messages msgs1;
		msgs1.set("key", "message");

		trogdor::Messages msgs2 = msgs1;
		CHECK(0 == msgs2.get("key").compare("message"));
	}
}
