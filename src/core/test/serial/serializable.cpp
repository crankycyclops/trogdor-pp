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

	TEST_CASE("Deserialization constuctor") {

		// TODO
		CHECK(true);
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

	TEST_CASE("clear() and size()") {

		trogdor::serial::Serializable s;

		CHECK(0 == s.size());

		s.set("key1", "string1");
		s.set("key2", "string2");

		CHECK(2 == s.size());

		s.clear();

		CHECK(0 == s.size());
	}

	TEST_CASE("set() and get()") {

		trogdor::serial::Serializable s;

		CHECK(std::nullopt == s.get("notAValue"));

		s.set("size_t", static_cast<size_t>(1));

		CHECK(std::nullopt != s.get("size_t"));
		CHECK(1 == std::get<size_t>(*s.get("size_t")));

		s.set("int", static_cast<int>(-1));

		CHECK(std::nullopt != s.get("int"));
		CHECK(-1 == std::get<int>(*s.get("int")));

		s.set("double", 1.1);

		CHECK(std::nullopt != s.get("double"));
		CHECK(1.1 == std::get<double>(*s.get("double")));

		s.set("true", true);

		CHECK(std::nullopt != s.get("true"));
		CHECK(true == std::get<bool>(*s.get("true")));

		s.set("false", false);

		CHECK(std::nullopt != s.get("false"));
		CHECK(false == std::get<bool>(*s.get("false")));

		s.set("string", "stringval");

		CHECK(std::nullopt != s.get("string"));
		CHECK(0 == std::get<std::string>(*s.get("string")).compare("stringval"));

		std::shared_ptr<trogdor::serial::Serializable> s2 =
			std::make_shared<trogdor::serial::Serializable>();

		s.set("serializable", s2);

		CHECK(std::nullopt != s.get("serializable"));

		// Doctest won't let me check this comparison directly
		bool validSetSerializable = s2 == std::get<std::shared_ptr<trogdor::serial::Serializable>>(*s.get("serializable"));
		CHECK(validSetSerializable);
	}

	TEST_CASE("getAll()") {

		trogdor::serial::Serializable s;

		// Make sure there are no entries initially
		for (const auto &pair: s.getAll()) {
			CHECK(false);
		}

		int numFound = 0;

		s.set("key1", "string");
		s.set("key2", true);

		for (const auto &pair: s.getAll()) {

			if (0 == pair.first.compare("key1")) {
				numFound++;
				CHECK(0 == std::get<std::string>(pair.second).compare("string"));
			}

			else if (0 == pair.first.compare("key2")) {
				numFound++;
				CHECK(true == std::get<bool>(pair.second));
			}

			else {
				CHECK(false);
			}
		}

		CHECK(2 == numFound);
	}

	TEST_CASE("erase()") {

		trogdor::serial::Serializable s;

		s.set("size_t", static_cast<size_t>(1));

		CHECK(std::nullopt != s.get("size_t"));
		CHECK(1 == std::get<size_t>(*s.get("size_t")));

		s.erase("size_t");

		CHECK(0 == s.size());
	}
}
