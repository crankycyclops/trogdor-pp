#include <doctest.h>

#include <trogdor/entities/room.h>
#include <trogdor/entities/object.h>
#include <trogdor/entities/creature.h>
#include <trogdor/entities/player.h>

#include <trogdor/iostream/nullout.h>
#include <trogdor/iostream/nullerr.h>


TEST_SUITE("Tangible (entities/tangible.cpp)") {

	// Tests to ensure that all physical entity types test true as instances of
	// Tangible
	TEST_CASE("Tangible (entities/tangible.cpp): Proper inheritance") {

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
	}
}
