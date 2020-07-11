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
std::unique_ptr<GlobalController> GlobalController::instance = nullptr;

/*****************************************************************************/

GlobalController::GlobalController() {

	registerAction(Request::GET, STATISTICS_ACTION, [&] (JSONObject request) -> JSONObject {
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

JSONObject GlobalController::statistics(JSONObject request) {

	JSONObject response;
	JSONObject version;
	JSONObject libVersion;

	version.put("major", TROGDORD_VERSION_MAJOR);
	version.put("minor", TROGDORD_VERSION_MINOR);
	version.put("patch", TROGDORD_VERSION_PATCH);

	libVersion.put("major", TROGDOR_VERSION_MAJOR);
	libVersion.put("minor", TROGDOR_VERSION_MINOR);
	libVersion.put("patch", TROGDOR_VERSION_PATCH);

	// TODO: add number of existing games after I change from ids to names
	response.put("status", Response::STATUS_SUCCESS);
	response.put("players", GameContainer::get()->getNumPlayers());
	response.add_child("version", version);
	response.add_child("lib_version", libVersion);

	return response;
}
