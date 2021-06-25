#include <trogdor/game.h>

#include <trogdord/scopes/object.h>
#include <trogdord/exception/entity/objectnotfound.h>


// Singleton instance of ObjectController
std::unique_ptr<ObjectController> ObjectController::instance;

/*****************************************************************************/

rapidjson::Value ObjectController::entityToJSONObject(
	trogdor::entity::Entity *ePtr,
	rapidjson::MemoryPoolAllocator<> &allocator) {

	rapidjson::Value object = ThingController::entityToJSONObject(ePtr, allocator);

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

/*****************************************************************************/

const char *ObjectController::getName() {

	return SCOPE;
}
