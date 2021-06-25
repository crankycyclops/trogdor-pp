#include <trogdor/game.h>

#include <trogdord/scopes/tangible.h>
#include <trogdord/exception/entity/tangiblenotfound.h>


// Singleton instance of TangibleController
std::unique_ptr<TangibleController> TangibleController::instance;

/*****************************************************************************/

rapidjson::Value TangibleController::entityToJSONObject(
	trogdor::entity::Entity *ePtr,
	rapidjson::MemoryPoolAllocator<> &allocator
) {

	rapidjson::Value tangible = EntityController::entityToJSONObject(ePtr, allocator);

	// TODO: add tangible-specific properties
	return tangible;
}

/*****************************************************************************/

trogdor::entity::Entity *TangibleController::getEntityPtr(
	std::unique_ptr<trogdor::Game> &game,
	std::string entityName
) {

	trogdor::entity::Entity *ePtr = game->getTangible(entityName).get();

	if (!ePtr) {
		throw TangibleNotFound();
	}

	return ePtr;
}

/*****************************************************************************/

std::vector<trogdor::entity::Entity *> TangibleController::getEntityPtrList(
	std::unique_ptr<trogdor::Game> &game
) {

	std::vector<trogdor::entity::Entity *> tangibles;

	for (const auto &tangible: game->getTangibles()) {
		tangibles.push_back(tangible.second.get());
	}

	return tangibles;
}

/*****************************************************************************/

TangibleController::TangibleController(): EntityController() {

}

/*****************************************************************************/

std::unique_ptr<TangibleController> &TangibleController::get() {

	if (!instance) {
		instance = std::unique_ptr<TangibleController>(new TangibleController());
	}

	return instance;
}

/*****************************************************************************/

const char *TangibleController::getName() {

	return SCOPE;
}
