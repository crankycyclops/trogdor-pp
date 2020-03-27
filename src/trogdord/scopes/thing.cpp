#include <trogdor/game.h>

#include "../include/scopes/thing.h"
#include "../include/exception/entity/thingnotfound.h"


// Scope name that should be used in requests
const char *ThingController::SCOPE = "thing";

// Singleton instance of ThingController
std::unique_ptr<ThingController> ThingController::instance = nullptr;

/*****************************************************************************/

JSONObject ThingController::entityToJSONObject(trogdor::entity::Entity *ePtr) {

	JSONObject thing = EntityController::entityToJSONObject(ePtr);

	// TODO: add thing-specific properties
	return thing;
}

/*****************************************************************************/

trogdor::entity::Entity *ThingController::getEntityPtr(
	std::unique_ptr<trogdor::Game> &game,
	std::string entityName
) {

	trogdor::entity::Entity *ePtr = game->getThing(entityName);

	if (!ePtr) {
		throw ThingNotFound();
	}

	return ePtr;
}

/*****************************************************************************/

const trogdor::entity::EntityMap ThingController::getEntityPtrList(
	std::unique_ptr<trogdor::Game> &game
) {

	return game->getThings();
}

/*****************************************************************************/

ThingController::ThingController(): EntityController() {

}

/*****************************************************************************/

std::unique_ptr<ThingController> &ThingController::get() {

	if (!instance) {
		instance = std::unique_ptr<ThingController>(new ThingController());
	}

	return instance;
}