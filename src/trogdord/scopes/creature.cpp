#include <trogdor/game.h>

#include <trogdord/scopes/creature.h>
#include <trogdord/exception/entity/creaturenotfound.h>


// Singleton instance of CreatureController
std::unique_ptr<CreatureController> CreatureController::instance;

/*****************************************************************************/

rapidjson::Value CreatureController::entityToJSONObject(
	trogdor::entity::Entity *ePtr,
	rapidjson::MemoryPoolAllocator<> &allocator
) {

	rapidjson::Value creature = BeingController::entityToJSONObject(ePtr, allocator);

	// TODO: add creature-specific properties
	return creature;
}

/*****************************************************************************/

trogdor::entity::Entity *CreatureController::getEntityPtr(
	std::unique_ptr<trogdor::Game> &game,
	std::string entityName
) {

	trogdor::entity::Entity *ePtr = game->getCreature(entityName).get();

	if (!ePtr) {
		throw CreatureNotFound();
	}

	return ePtr;
}

/*****************************************************************************/

std::vector<trogdor::entity::Entity *> CreatureController::getEntityPtrList(
	std::unique_ptr<trogdor::Game> &game
) {

	std::vector<trogdor::entity::Entity *> creatures;

	for (const auto &creature: game->getCreatures()) {
		creatures.push_back(creature.second.get());
	}

	return creatures;
}

/*****************************************************************************/

CreatureController::CreatureController(): BeingController() {

}

/*****************************************************************************/

std::unique_ptr<CreatureController> &CreatureController::get() {

	if (!instance) {
		instance = std::unique_ptr<CreatureController>(new CreatureController());
	}

	return instance;
}

/*****************************************************************************/

const char *CreatureController::getName() {

	return SCOPE;
}
