#include <trogdor/version.h>

#include "../include/version.h"
#include "../include/request.h"
#include "../include/gamecontainer.h"
#include "../include/scopes/global.h"


// Singleton instance of GlobalController
std::unique_ptr<GlobalController> GlobalController::instance;

/*****************************************************************************/

GlobalController::GlobalController() {

	registerAction(Request::GET, STATISTICS_ACTION, [&] (const rapidjson::Document &request) -> rapidjson::Document {
		return this->statistics(request);
	});

	registerAction(Request::POST, DUMP_ACTION, [&] (const rapidjson::Document &request) -> rapidjson::Document {
		return this->dump(request);
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

	rapidjson::Document response(rapidjson::kObjectType);

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

/*****************************************************************************/

rapidjson::Document GlobalController::dump(const rapidjson::Document &request) {

	rapidjson::Document response(rapidjson::kObjectType);

	if (!Config::get()->getBool(Config::CONFIG_KEY_STATE_ENABLED)) {
		response.AddMember("status", Response::STATUS_UNSUPPORTED, response.GetAllocator());
		response.AddMember("message", rapidjson::StringRef(STATE_DISABLED), response.GetAllocator());
	}

	else {

		try {
			GameContainer::get()->dump();
			response.AddMember("status", Response::STATUS_SUCCESS, response.GetAllocator());
		}

		catch (const std::exception &e) {
			response.AddMember("status", Response::STATUS_INTERNAL_ERROR, response.GetAllocator());
			response.AddMember("message", rapidjson::StringRef(e.what()), response.GetAllocator());
		}
	}

	return response;
}
