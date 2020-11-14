#include <doctest.h>

#include <trogdor/iostream/nullout.h>
#include <trogdor/iostream/nullerr.h>

#include "../mock/mockentity.h"


TEST_SUITE("Entity (entities/entity.cpp)") {

	TEST_CASE("Entity (entities/entity.cpp): setProperty(), getProperty(), and unsetProperty()") {

		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());
		trogdor::entity::MockEntity testEntity(&mockGame, "test");

		// Test 1: Test retrieving non-existent property
		CHECK(!testEntity.isPropertySet("nope"));

		try {
			testEntity.getProperty<std::string>("nope");
			CHECK(false);
		} catch (const std::invalid_argument &e) {
			CHECK(true);
		}

		// Test 2: Test setting and retrieving each type without validation
		testEntity.setProperty("size_t", static_cast<size_t>(100));

		CHECK(testEntity.isPropertySet("size_t"));
		CHECK(100 == testEntity.getProperty<size_t>("size_t"));

		// Test access with the wrong type (only going to have one test case)
		try {
			testEntity.getProperty<std::string>("size_t");
			CHECK(false);
		} catch (const std::bad_variant_access &e) {
			CHECK(true);
		}

		testEntity.setProperty("int", -1);

		CHECK(testEntity.isPropertySet("int"));
		CHECK(-1 == testEntity.getProperty<int>("int"));

		testEntity.setProperty("double", -1.5);

		CHECK(testEntity.isPropertySet("double"));
		CHECK(-1.5 == testEntity.getProperty<double>("double"));

		testEntity.setProperty("bool", true);

		CHECK(testEntity.isPropertySet("bool"));
		CHECK(true == testEntity.getProperty<bool>("bool"));

		testEntity.setProperty("string1", "I'm a string");

		CHECK(testEntity.isPropertySet("string1"));
		CHECK(0 == testEntity.getProperty<std::string>("string1").compare("I'm a string"));

		testEntity.setProperty("string2", std::string("I'm a C++ string"));

		CHECK(testEntity.isPropertySet("string2"));
		CHECK(0 == testEntity.getProperty<std::string>("string2").compare("I'm a C++ string"));

		// Test 3: Test unsetting properties
		testEntity.unsetProperty("size_t");
		CHECK(!testEntity.isPropertySet("size_t"));

		testEntity.unsetProperty("int");
		CHECK(!testEntity.isPropertySet("int"));

		testEntity.unsetProperty("double");
		CHECK(!testEntity.isPropertySet("double"));

		testEntity.unsetProperty("bool");
		CHECK(!testEntity.isPropertySet("bool"));

		testEntity.unsetProperty("string1");
		CHECK(!testEntity.isPropertySet("string1"));

		testEntity.unsetProperty("string2");
		CHECK(!testEntity.isPropertySet("string2"));

		// Test 4: Overwrite value with same type
		testEntity.setProperty("intval", 1);
		CHECK(1 == testEntity.getProperty<int>("intval"));

		testEntity.setProperty("intval", 2);
		CHECK(2 == testEntity.getProperty<int>("intval"));

		// Test 4: Overwrite value with different type
		testEntity.setProperty("mixed", 10);
		CHECK(10 == testEntity.getProperty<int>("mixed"));

		testEntity.setProperty("mixed", "wee");
		CHECK(0 == testEntity.getProperty<std::string>("mixed").compare("wee"));

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
		CHECK(testEntity.isPropertySet("intValidated"));
		CHECK(10 == testEntity.getProperty<int>("intValidated"));

		// Reset property to unset state before trying again to show the value
		// doesn't get set if it's invalid
		testEntity.unsetProperty("intValidated");

		// Test 7: Setting property with failing validation should NOT return
		// PROPERTY_VALID and should result in the property NOT being set
		status = testEntity.setProperty("intValidated", "I'm a string");
		CHECK(-1 == status);
		CHECK(!testEntity.isPropertySet("intValidated"));
	}
}
