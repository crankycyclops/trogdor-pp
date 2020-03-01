#include <trogdor/game.h>

#include "../include/scopes/object.h"
#include "../include/exception/entity/objectnotfound.h"


// Scope name that should be used in requests
const char *ObjectController::SCOPE = "object";

// Singleton instance of ObjectController
std::unique_ptr<ObjectController> ObjectController::instance = nullptr;

/*****************************************************************************/

JSONObject ObjectController::entityToJSONObject(trogdor::entity::Entity *ePtr) {

	JSONObject object = ThingController::entityToJSONObject(ePtr);

	// TODO: add object-specific properties
	return object;
}

/*****************************************************************************/

trogdor::entity::Entity *ObjectController::getEntityPtr(
	std::unique_ptr<trogdor::Game> &game,
	std::string entityName
) {

	trogdor::entity::Entity *ePtr = game->getObject(entityName);

	if (!ePtr) {
		throw ObjectNotFound();
	}

	return ePtr;
}

/*****************************************************************************/

const trogdor::entity::EntityMap ObjectController::getEntityPtrList(
	std::unique_ptr<trogdor::Game> &game
) {

	return game->getObjects();
}

/*****************************************************************************/

ObjectController::ObjectController(): ThingController() {

}

/*****************************************************************************/

std::unique_ptr<ObjectController> &ObjectController::get() {

	if (!instance) {
		instance = std::unique_ptr<ObjectController>(new ObjectController());
	}

	return instance;
}
