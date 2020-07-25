#include <doctest.h>

#include <trogdor/event/triggers/deathdrop.h>

#include <trogdor/entities/room.h>
#include <trogdor/entities/object.h>
#include <trogdor/entities/creature.h>

#include <trogdor/iostream/nullout.h>
#include <trogdor/iostream/nullerr.h>


TEST_SUITE("DeathDropEventTrigger (event/triggers/deathdrop.cpp)") {

	// Their stuff should NOT be dropped
	TEST_CASE("DeathDropEventTrigger (event/triggers/deathdrop.cpp): Being is alive") {

		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());

		std::shared_ptr<trogdor::entity::Room> aRoom =
		std::make_shared<trogdor::entity::Room>(
			&mockGame,
			"start",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		std::shared_ptr<trogdor::entity::Object> anObject =
		std::make_shared<trogdor::entity::Object>(
			&mockGame,
			"sword",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		std::shared_ptr<trogdor::entity::Creature> aGuy =
		std::make_shared<trogdor::entity::Creature>(
			&mockGame,
			"alive_guy",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		aGuy->insertIntoInventory(anObject);
		aRoom->insertCreature(aGuy);

		auto invObjects = aGuy->getInventoryObjects();
		auto thingsInRoom = aRoom->getCreatures();

		// Verify that the Object was added to the Creature's inventory
		CHECK(1 == invObjects.size());
		CHECK(anObject.get() == (*invObjects.begin()).get());
		CHECK(anObject->getOwner().lock());
		CHECK(aGuy.get() == anObject->getOwner().lock().get());

		// Verify that the Creature was added to the Room
		CHECK(1 == thingsInRoom.size());
		CHECK(aGuy.get() == (*thingsInRoom.begin()).get());
		CHECK(aGuy->getLocation().lock());
		CHECK(aRoom.get() == aGuy->getLocation().lock().get());

		trogdor::event::DeathDropEventTrigger deathTrigger;
		auto result = deathTrigger({"test", {}, {&mockGame, aGuy.get()}});

		auto invObjectsFinal = aGuy->getInventoryObjects();
		auto thingsInRoomFinal = aRoom->getCreatures();

		// Make sure DeathDropEventTrigger allows the action and continues
		// execution of remaining events
		CHECK(result.allowAction);
		CHECK(result.continueExecution);

		// Verify that the Object still belongs to the Creature and that it
		// hasn't been dropped in the Room
		CHECK(1 == invObjectsFinal.size());
		CHECK(anObject.get() == (*invObjectsFinal.begin()).get());
		CHECK(anObject->getOwner().lock());
		CHECK(aGuy.get() == anObject->getOwner().lock().get());

		// Verify that the Creature was added to the Room
		CHECK(1 == thingsInRoomFinal.size());
		CHECK(aGuy.get() == (*thingsInRoomFinal.begin()).get());
		CHECK(aGuy->getLocation().lock());
		CHECK(aRoom.get() == aGuy->getLocation().lock().get());
	}

	// Their stuff SHOULD be dropped
	TEST_CASE("DeathDropEventTrigger (event/triggers/deathdrop.cpp): Being is dead") {

		trogdor::Game mockGame(std::make_unique<trogdor::NullErr>());

		std::shared_ptr<trogdor::entity::Room> aRoom =
		std::make_shared<trogdor::entity::Room>(
			&mockGame,
			"start",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		std::shared_ptr<trogdor::entity::Object> anObject =
		std::make_shared<trogdor::entity::Object>(
			&mockGame,
			"sword",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		std::shared_ptr<trogdor::entity::Creature> aGuy =
		std::make_shared<trogdor::entity::Creature>(
			&mockGame,
			"alive_guy",
			std::make_unique<trogdor::NullOut>(),
			std::make_unique<trogdor::NullErr>()
		);

		// Until we set a max health, a Being is considered immortal
		aGuy->setHealth(10), aGuy->setMaxHealth(10);
		aGuy->die();

		aGuy->insertIntoInventory(anObject);
		aRoom->insertCreature(aGuy);

		auto invObjects = aGuy->getInventoryObjects();
		auto thingsInRoom = aRoom->getCreatures();

		// Verify that the Object was added to the Creature's inventory
		CHECK(1 == invObjects.size());
		CHECK(anObject.get() == (*invObjects.begin()).get());
		CHECK(anObject->getOwner().lock());
		CHECK(aGuy.get() == anObject->getOwner().lock().get());

		// Verify that the Creature was added to the Room
		CHECK(1 == thingsInRoom.size());
		CHECK(aGuy.get() == (*thingsInRoom.begin()).get());
		CHECK(aGuy->getLocation().lock());
		CHECK(aRoom.get() == aGuy->getLocation().lock().get());

		trogdor::event::DeathDropEventTrigger deathTrigger;
		auto result = deathTrigger({"test", {}, {&mockGame, aGuy.get()}});

		auto invObjectsFinal = aGuy->getInventoryObjects();
		auto objectsInRoomFinal = aRoom->getObjects();

		// Make sure DeathDropEventTrigger allows the action and continues
		// execution of remaining events
		CHECK(result.allowAction);
		CHECK(result.continueExecution);

		// Verify that the Object is no longer in the Creature's inventory
		CHECK(0 == invObjectsFinal.size());

		// Verify that the Object is now in the Room along with the dead Creature
		CHECK(1 == objectsInRoomFinal.size());
		CHECK(anObject.get() == (*objectsInRoomFinal.begin()).get());
		CHECK(anObject->getLocation().lock());
		CHECK(aRoom.get() == anObject->getLocation().lock().get());
	}
}
