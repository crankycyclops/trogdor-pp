#include <trogdor/game.h>

#include "../include/scopes/place.h"
#include "../include/exception/entity/placenotfound.h"


// Scope name that should be used in requests
const char *PlaceController::SCOPE = "place";

// Singleton instance of PlaceController
std::unique_ptr<PlaceController> PlaceController::instance = nullptr;

/*****************************************************************************/

JSONObject PlaceController::entityToJSONObject(trogdor::entity::Entity *ePtr) {

	JSONObject place = EntityController::entityToJSONObject(ePtr);

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
