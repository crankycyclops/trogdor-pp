#include <trogdor/game.h>

#include "../include/scopes/resource.h"
#include "../include/exception/entity/resourcenotfound.h"


// Scope name that should be used in requests
const char *ResourceController::SCOPE = "resource";

// Singleton instance of ResourceController
std::unique_ptr<ResourceController> ResourceController::instance = nullptr;

/*****************************************************************************/

JSONObject ResourceController::entityToJSONObject(trogdor::entity::Entity *ePtr) {

	JSONObject resource = EntityController::entityToJSONObject(ePtr);

	// TODO: add resource-specific properties
	return resource;
}

/*****************************************************************************/

trogdor::entity::Entity *ResourceController::getEntityPtr(
	std::unique_ptr<trogdor::Game> &game,
	std::string entityName
) {

	trogdor::entity::Entity *ePtr = game->getResource(entityName).get();

	if (!ePtr) {
		throw ResourceNotFound();
	}

	return ePtr;
}

/*****************************************************************************/

std::vector<trogdor::entity::Entity *> ResourceController::getEntityPtrList(
	std::unique_ptr<trogdor::Game> &game
) {

	std::vector<trogdor::entity::Entity *> resources;

	for (const auto &resource: game->getResources()) {
		resources.push_back(resource.second.get());
	}

	return resources;
}

/*****************************************************************************/

ResourceController::ResourceController(): EntityController() {

}

/*****************************************************************************/

std::unique_ptr<ResourceController> &ResourceController::get() {

	if (!instance) {
		instance = std::unique_ptr<ResourceController>(new ResourceController());
	}

	return instance;
}
