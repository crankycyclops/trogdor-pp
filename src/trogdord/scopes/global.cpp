#include "../include/version.h"
#include "../include/request.h"
#include "../include/scopes/global.h"


// Scope name that should be used in requests
const char *Global::SCOPE = "global";

// Action names that get mapped to methods in Global
const char *Global::STATISTICS_ACTION = "statistics";

// Singleton instance of Global
std::unique_ptr<Global> Global::instance = nullptr;

/*****************************************************************************/

Global::Global() {

	registerAction(Request::GET, STATISTICS_ACTION, [&] (JSONObject request) -> JSONObject {
		return this->statistics(request);
	});
}

/*****************************************************************************/

std::unique_ptr<Global> &Global::get() {

	if (!instance) {
		instance = std::unique_ptr<Global>(new Global());
	}

	return instance;
}

/*****************************************************************************/

JSONObject Global::statistics(JSONObject request) {

	JSONObject response;
	JSONObject version;

	version.put("major", TROGDORD_VERSION_MAJOR);
	version.put("minor", TROGDORD_VERSION_MINOR);
	version.put("patch", TROGDORD_VERSION_PATCH);

	// TODO: add number of existing games and other stats
	response.put("status", 200);
	response.add_child("version", version);

	return response;
}
