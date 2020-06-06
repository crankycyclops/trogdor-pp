#include <trogdor/game.h>

#include "../include/scopes/creature.h"
#include "../include/exception/entity/creaturenotfound.h"


// Scope name that should be used in requests
const char *CreatureController::SCOPE = "creature";

// Singleton instance of CreatureController
std::unique_ptr<CreatureController> CreatureController::instance = nullptr;

/*****************************************************************************/

JSONObject CreatureController::entityToJSONObject(trogdor::entity::Entity *ePtr) {

	JSONObject creature = BeingController::entityToJSONObject(ePtr);

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
