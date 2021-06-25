#include <trogdor/game.h>

#include <trogdord/scopes/thing.h>
#include <trogdord/exception/entity/thingnotfound.h>


// Singleton instance of ThingController
std::unique_ptr<ThingController> ThingController::instance;

/*****************************************************************************/

rapidjson::Value ThingController::entityToJSONObject(
	trogdor::entity::Entity *ePtr,
	rapidjson::MemoryPoolAllocator<> &allocator
) {

	rapidjson::Value thing = EntityController::entityToJSONObject(ePtr, allocator);

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

/*****************************************************************************/

const char *ThingController::getName() {

	return SCOPE;
}
