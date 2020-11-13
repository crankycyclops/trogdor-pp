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

		// Test 4: Overwrite value with same type
		testEntity.setProperty("intval", 1);
		CHECK(1 == std::get<int>(*testEntity.getProperty("intval")));

		testEntity.setProperty("intval", 2);
		CHECK(2 == std::get<int>(*testEntity.getProperty("intval")));

		// Test 4: Overwrite value with different type
		testEntity.setProperty("mixed", 10);
		CHECK(10 == std::get<int>(*testEntity.getProperty("mixed")));

		testEntity.setProperty("mixed", "wee");
		CHECK(0 == std::get<std::string>(*testEntity.getProperty("mixed")).compare("wee"));

		// Test 5: Setting property with no validator should result in return
		// value of PROPERTY_VALID
		int status = testEntity.setProperty("novalidator", static_cast<int>(0));
		CHECK(status == trogdor::entity::Entity::PROPERTY_VALID);

		// Only allows an integer value to be set
		testEntity.setPropertyValidator(
			"intValidated",
			[](trogdor::entity::Entity::PropertyValue v) -> int {
				return 1 == v.index() ? trogdor::entity::MockEntity::PROPERTY_VALID : -1;
			}
		);

		// Test 6: Setting property with passing validation should return
		// PROPERTY_VALID and the property should be set
		status = testEntity.setProperty("intValidated", 10);
		CHECK(trogdor::entity::MockEntity::PROPERTY_VALID == status);
		CHECK(std::nullopt != testEntity.getProperty("intValidated"));
		CHECK(10 == std::get<int>(*testEntity.getProperty("intValidated")));

		// Reset property to unset state before trying again to show the value
		// doesn't get set if it's invalid
		testEntity.unsetProperty("intValidated");

		// Test 7: Setting property with failing validation should NOT return
		// PROPERTY_VALID and should result in the property NOT being set
		status = testEntity.setProperty("intValidated", "I'm a string");
		CHECK(-1 == status);
		CHECK(std::nullopt == testEntity.getProperty("intValidated"));
	}
}
