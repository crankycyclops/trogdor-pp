#include <trogdor/version.h>

#include "../include/version.h"
#include "../include/request.h"
#include "../include/gamecontainer.h"
#include "../include/scopes/global.h"


// Scope name that should be used in requests
const char *GlobalController::SCOPE = "global";

// Action names that get mapped to methods in GlobalController
const char *GlobalController::STATISTICS_ACTION = "statistics";

// Singleton instance of GlobalController
std::unique_ptr<GlobalController> GlobalController::instance;

/*****************************************************************************/

GlobalController::GlobalController() {

	registerAction(Request::GET, STATISTICS_ACTION, [&] (const rapidjson::Document &request) -> rapidjson::Document {
		return this->statistics(request);
	});
}

/*****************************************************************************/

std::unique_ptr<GlobalController> &GlobalController::get() {

	if (!instance) {
		instance = std::unique_ptr<GlobalController>(new GlobalController());
	}

	return instance;
}

/*****************************************************************************/

rapidjson::Document GlobalController::statistics(const rapidjson::Document &request) {

	rapidjson::Document response;

	rapidjson::Value version(rapidjson::kObjectType);
	rapidjson::Value libVersion(rapidjson::kObjectType);

	version.AddMember("major", TROGDORD_VERSION_MAJOR, response.GetAllocator());
	version.AddMember("minor", TROGDORD_VERSION_MINOR, response.GetAllocator());
	version.AddMember("patch", TROGDORD_VERSION_PATCH, response.GetAllocator());

	libVersion.AddMember("major", TROGDOR_VERSION_MAJOR, response.GetAllocator());
	libVersion.AddMember("minor", TROGDOR_VERSION_MINOR, response.GetAllocator());
	libVersion.AddMember("patch", TROGDOR_VERSION_PATCH, response.GetAllocator());

	// TODO: add number of existing games after I change from ids to names
	response.AddMember("status", Response::STATUS_SUCCESS, response.GetAllocator());
	response.AddMember("players", GameContainer::get()->getNumPlayers(), response.GetAllocator());
	response.AddMember("version", version, response.GetAllocator());
	response.AddMember("lib_version", libVersion, response.GetAllocator());

	return response;
}
