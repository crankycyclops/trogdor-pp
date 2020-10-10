#include <trogdor/game.h>

#include "../include/scopes/thing.h"
#include "../include/exception/entity/thingnotfound.h"


// Scope name that should be used in requests
const char *ThingController::SCOPE = "thing";

// Singleton instance of ThingController
std::unique_ptr<ThingController> ThingController::instance;

/*****************************************************************************/

rapidjson::Document ThingController::entityToJSONObject(trogdor::entity::Entity *ePtr) {

	rapidjson::Document thing = EntityController::entityToJSONObject(ePtr);

	// TODO: add thing-specific properties
	return thing;
}

/*****************************************************************************/

trogdor::entity::Entity *ThingController::getEntityPtr(
	std::unique_ptr<trogdor::Game> &game,
	std::string entityName
) {

	trogdor::entity::Entity *ePtr = game->getThing(entityName).get();

	if (!ePtr) {
		throw ThingNotFound();
	}

	return ePtr;
}

/*****************************************************************************/

std::vector<trogdor::entity::Entity *> ThingController::getEntityPtrList(
	std::unique_ptr<trogdor::Game> &game
) {

	std::vector<trogdor::entity::Entity *> things;

	for (const auto &thing: game->getThings()) {
		things.push_back(thing.second.get());
	}

	return things;
}

/*****************************************************************************/

ThingController::ThingController(): TangibleController() {

}

/*****************************************************************************/

std::unique_ptr<ThingController> &ThingController::get() {

	if (!instance) {
		instance = std::unique_ptr<ThingController>(new ThingController());
	}

	return instance;
}
