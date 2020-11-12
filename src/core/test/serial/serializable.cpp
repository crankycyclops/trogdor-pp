#include <doctest.h>
#include <trogdor/serial/serializable.h>


TEST_SUITE("Serializable (serializable.h)") {

	TEST_CASE("Default constructor (should create empty container)") {

		trogdor::serial::Serializable s;
		CHECK(0 == s.size());
	}

	TEST_CASE("Copy constructor and assignment operator") {

		trogdor::serial::Serializable s1;

		// Not explicitly casting an unsigned integer to size_t will cause it
		// to be set as an ordinary int instead.
		s1.set("key", static_cast<size_t>(1));

		CHECK(1 == s1.size());
		CHECK(std::nullopt != s1.get("key"));
		CHECK(1 == std::get<size_t>(*s1.get("key")));

		trogdor::serial::Serializable s2(s1);

		CHECK(1 == s2.size());
		CHECK(std::nullopt != s2.get("key"));
		CHECK(1 == std::get<size_t>(*s2.get("key")));
	}

	TEST_CASE("Assignment operator") {

		trogdor::serial::Serializable s1;

		s1.set("key", false);

		CHECK(1 == s1.size());
		CHECK(std::nullopt != s1.get("key"));
		CHECK(false == std::get<bool>(*s1.get("key")));

		trogdor::serial::Serializable s2;

		s2 = s1;

		CHECK(1 == s2.size());
		CHECK(std::nullopt != s2.get("key"));
		CHECK(false == std::get<bool>(*s2.get("key")));
	}
}
