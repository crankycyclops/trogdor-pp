#include <trogdor/game.h>

#include <trogdord/scopes/being.h>
#include <trogdord/exception/entity/beingnotfound.h>


// Singleton instance of BeingController
std::unique_ptr<BeingController> BeingController::instance;

/*****************************************************************************/

rapidjson::Value BeingController::entityToJSONObject(
	trogdor::entity::Entity *ePtr,
	rapidjson::MemoryPoolAllocator<> &allocator
) {

	rapidjson::Value being = ThingController::entityToJSONObject(ePtr, allocator);

	// TODO: add being-specific properties
	return being;
}

/*****************************************************************************/

trogdor::entity::Entity *BeingController::getEntityPtr(
	std::unique_ptr<trogdor::Game> &game,
	std::string entityName
) {

	trogdor::entity::Entity *ePtr = game->getBeing(entityName).get();

	if (!ePtr) {
		throw BeingNotFound();
	}

	return ePtr;
}

/*****************************************************************************/

std::vector<trogdor::entity::Entity *> BeingController::getEntityPtrList(
	std::unique_ptr<trogdor::Game> &game
) {

	std::vector<trogdor::entity::Entity *> beings;

	for (const auto &being: game->getBeings()) {
		beings.push_back(being.second.get());
	}

	return beings;
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

/*****************************************************************************/

const char *BeingController::getName() {

	return SCOPE;
}
