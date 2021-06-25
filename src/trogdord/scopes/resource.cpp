#include <trogdor/game.h>

#include <trogdord/scopes/resource.h>
#include <trogdord/exception/entity/resourcenotfound.h>


// Singleton instance of ResourceController
std::unique_ptr<ResourceController> ResourceController::instance;

/*****************************************************************************/

rapidjson::Value ResourceController::entityToJSONObject(
	trogdor::entity::Entity *ePtr,
	rapidjson::MemoryPoolAllocator<> &allocator
) {

	rapidjson::Value resource = EntityController::entityToJSONObject(ePtr, allocator);

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

/*****************************************************************************/

const char *ResourceController::getName() {

	return SCOPE;
}
