#include <doctest.h>

#include <trogdor/iostream/nullout.h>
#include <trogdor/iostream/nullerr.h>

#include "../mock/mockentity.h"


TEST_SUITE("Entity (entities/entity.cpp)") {

	TEST_CASE("Entity (entities/entity.cpp): setProperty(), getProperty(), and unsetProperty()") {

		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());
		trogdor::entity::MockEntity testEntity(&mockGame, "test");

		// Test 1: Test retrieving non-existent property
		CHECK(std::nullopt == testEntity.getProperty("nope"));

		// Test 2: Test setting and retrieving each type without validation
		testEntity.setProperty("size_t", static_cast<size_t>(100));

		CHECK(std::nullopt != testEntity.getProperty("size_t"));
		CHECK(0 == testEntity.getProperty("size_t")->index()); // 0 is the index for size_t
		CHECK(100 == std::get<size_t>(*testEntity.getProperty("size_t")));

		testEntity.setProperty("int", -1);

		CHECK(std::nullopt != testEntity.getProperty("int"));
		CHECK(1 == testEntity.getProperty("int")->index());
		CHECK(-1 == std::get<int>(*testEntity.getProperty("int")));

		testEntity.setProperty("double", -1.5);

		CHECK(std::nullopt != testEntity.getProperty("double"));
		CHECK(2 == testEntity.getProperty("double")->index());
		CHECK(-1.5 == std::get<double>(*testEntity.getProperty("double")));

		testEntity.setProperty("bool", true);

		CHECK(std::nullopt != testEntity.getProperty("bool"));
		CHECK(3 == testEntity.getProperty("bool")->index());
		CHECK(true == std::get<bool>(*testEntity.getProperty("bool")));

		testEntity.setProperty("string1", "I'm a string");

		CHECK(std::nullopt != testEntity.getProperty("string1"));
		CHECK(4 == testEntity.getProperty("string1")->index());
		CHECK(0 == std::get<std::string>(*testEntity.getProperty("string1")).compare("I'm a string"));

		testEntity.setProperty("string2", std::string("I'm a C++ string"));

		CHECK(std::nullopt != testEntity.getProperty("string2"));
		CHECK(4 == testEntity.getProperty("string2")->index());
		CHECK(0 == std::get<std::string>(*testEntity.getProperty("string2")).compare("I'm a C++ string"));

		// Test 3: Test unsetting properties
		testEntity.unsetProperty("size_t");
		CHECK(std::nullopt == testEntity.getProperty("size_t"));

		testEntity.unsetProperty("int");
		CHECK(std::nullopt == testEntity.getProperty("int"));

		testEntity.unsetProperty("double");
		CHECK(std::nullopt == testEntity.getProperty("double"));

		testEntity.unsetProperty("bool");
		CHECK(std::nullopt == testEntity.getProperty("bool"));

		testEntity.unsetProperty("string1");
		CHECK(std::nullopt == testEntity.getProperty("string1"));

		testEntity.unsetProperty("string2");
		CHECK(std::nullopt == testEntity.getProperty("string2"));

		// TODO: test overwriting value with same type, diff value
		// TODO: test overwriting value with different type

		// TODO: test with validators
	}
}
