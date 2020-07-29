#include <doctest.h>

#include <trogdor/entities/room.h>
#include <trogdor/entities/object.h>
#include <trogdor/entities/creature.h>
#include <trogdor/entities/player.h>

#include <trogdor/iostream/nullout.h>
#include <trogdor/iostream/nullerr.h>


// Since Tangible's resources are managed by Resource via friend functions, the
// bulk of the unit tests for that functionality will be done in the Resource
// test suite (test/resource.cpp.)
TEST_SUITE("Tangible (entities/tangible.cpp)") {

	// Tests for sane construction of all derived types
	TEST_CASE("Tangible (entities/tangible.cpp): Sane Construction and Inheritance") {

		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());

		trogdor::entity::Room testRoom(
			&mockGame,
			"start",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		trogdor::entity::Object testObject(
			&mockGame,
			"sword",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		trogdor::entity::Creature testCreature(
			&mockGame,
			"trogdor",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		trogdor::entity::Player testPlayer(
			&mockGame,
			"player",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		CHECK(testRoom.isType(trogdor::entity::ENTITY_TANGIBLE));
		CHECK(testObject.isType(trogdor::entity::ENTITY_TANGIBLE));
		CHECK(testCreature.isType(trogdor::entity::ENTITY_TANGIBLE));
		CHECK(testPlayer.isType(trogdor::entity::ENTITY_TANGIBLE));

		CHECK(0 == testRoom.getResources().size());
		CHECK(0 == testObject.getResources().size());
		CHECK(0 == testCreature.getResources().size());
		CHECK(0 == testPlayer.getResources().size());
	}
}
