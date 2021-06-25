#include <trogdor/game.h>

#include <trogdord/scopes/place.h>
#include <trogdord/exception/entity/placenotfound.h>


// Singleton instance of PlaceController
std::unique_ptr<PlaceController> PlaceController::instance;

/*****************************************************************************/

rapidjson::Value PlaceController::entityToJSONObject(
	trogdor::entity::Entity *ePtr,
	rapidjson::MemoryPoolAllocator<> &allocator
) {

	rapidjson::Value place = EntityController::entityToJSONObject(ePtr, allocator);

	// TODO: add place-specific properties
	return place;
}

/*****************************************************************************/

trogdor::entity::Entity *PlaceController::getEntityPtr(
	std::unique_ptr<trogdor::Game> &game,
	std::string entityName
) {

	trogdor::entity::Entity *ePtr = game->getPlace(entityName).get();

	if (!ePtr) {
		throw PlaceNotFound();
	}

	return ePtr;
}

/*****************************************************************************/

std::vector<trogdor::entity::Entity *> PlaceController::getEntityPtrList(
	std::unique_ptr<trogdor::Game> &game
) {

	std::vector<trogdor::entity::Entity *> places;

	for (const auto &place: game->getPlaces()) {
		places.push_back(place.second.get());
	}

	return places;
}

/*****************************************************************************/

PlaceController::PlaceController(): TangibleController() {

}

/*****************************************************************************/

std::unique_ptr<PlaceController> &PlaceController::get() {

	if (!instance) {
		instance = std::unique_ptr<PlaceController>(new PlaceController());
	}

	return instance;
}

/*****************************************************************************/

const char *PlaceController::getName() {

	return SCOPE;
}
