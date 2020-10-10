#include <trogdor/game.h>

#include "../include/scopes/object.h"
#include "../include/exception/entity/objectnotfound.h"


// Scope name that should be used in requests
const char *ObjectController::SCOPE = "object";

// Singleton instance of ObjectController
std::unique_ptr<ObjectController> ObjectController::instance;

/*****************************************************************************/

rapidjson::Document ObjectController::entityToJSONObject(trogdor::entity::Entity *ePtr) {

	rapidjson::Document object = ThingController::entityToJSONObject(ePtr);

	// TODO: add object-specific properties
	return object;
}

/*****************************************************************************/

trogdor::entity::Entity *ObjectController::getEntityPtr(
	std::unique_ptr<trogdor::Game> &game,
	std::string entityName
) {

	trogdor::entity::Entity *ePtr = game->getObject(entityName).get();

	if (!ePtr) {
		throw ObjectNotFound();
	}

	return ePtr;
}

/*****************************************************************************/

std::vector<trogdor::entity::Entity *> ObjectController::getEntityPtrList(
	std::unique_ptr<trogdor::Game> &game
) {

	std::vector<trogdor::entity::Entity *> objects;

	for (const auto &object: game->getObjects()) {
		objects.push_back(object.second.get());
	}

	return objects;
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
