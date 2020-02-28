#include <trogdor/game.h>

#include "../include/scopes/being.h"
#include "../include/exception/entity/beingnotfound.h"


// Scope name that should be used in requests
const char *BeingController::SCOPE = "being";

// Singleton instance of BeingController
std::unique_ptr<BeingController> BeingController::instance = nullptr;

/*****************************************************************************/

JSONObject BeingController::entityToJSONObject(trogdor::entity::Entity *ePtr) {

	JSONObject being = ThingController::entityToJSONObject(ePtr);

	// TODO: add being-specific properties
	return being;
}

/*****************************************************************************/

trogdor::entity::Entity *BeingController::getEntityPtr(
	std::unique_ptr<trogdor::Game> &game,
	std::string entityName
) {

	trogdor::entity::Entity *ePtr = game->getBeing(entityName);

	if (!ePtr) {
		throw BeingNotFound();
	}

	return ePtr;
}

/*****************************************************************************/

const trogdor::entity::EntityMap BeingController::getEntityPtrList(
	std::unique_ptr<trogdor::Game> &game
) {

	return game->getBeings();
}

/*****************************************************************************/

BeingController::BeingController(): ThingController() {

}

/*****************************************************************************/

std::unique_ptr<BeingController> &BeingController::get() {

	if (!instance) {
		instance = std::unique_ptr<BeingController>(new BeingController());
	}

	return instance;
}
