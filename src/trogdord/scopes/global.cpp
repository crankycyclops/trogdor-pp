#include <trogdor/version.h>

#include <trogdord/version.h>
#include <trogdord/request.h>
#include <trogdord/gamecontainer.h>
#include <trogdord/scopes/global.h>


// Singleton instance of GlobalController
std::unique_ptr<GlobalController> GlobalController::instance;

/*****************************************************************************/

GlobalController::GlobalController() {

	registerAction(Request::GET, CONFIG_ACTION, [&] (const rapidjson::Document &request) -> rapidjson::Document {
		return this->config(request);
	});

	registerAction(Request::GET, STATISTICS_ACTION, [&] (const rapidjson::Document &request) -> rapidjson::Document {
		return this->statistics(request);
	});

	registerAction(Request::POST, DUMP_ACTION, [&] (const rapidjson::Document &request) -> rapidjson::Document {
		return this->dump(request);
	});

	registerAction(Request::POST, RESTORE_ACTION, [&] (const rapidjson::Document &request) -> rapidjson::Document {
		return this->restore(request);
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

const char *GlobalController::getName() {

	return SCOPE;
}

/*****************************************************************************/

rapidjson::Document GlobalController::config(const rapidjson::Document &request) {

	rapidjson::Document response(rapidjson::kObjectType);
	rapidjson::Value configVals(rapidjson::kObjectType);

	for (const auto &setting: *Config::get()) {

		// Only expose config settings that don't contain sensitive information
		if (auto i = Config::hidden.find(setting.first); i != Config::hidden.end() && !i->second) {

			if (typeid(int) == *Config::types.find(setting.first)->second) {
				configVals.AddMember(
					rapidjson::StringRef(setting.first.c_str()),
					Config::get()->getInt(setting.first),
					response.GetAllocator()
				);
			}

			else if (typeid(bool) == *Config::types.find(setting.first)->second) {
				configVals.AddMember(
					rapidjson::StringRef(setting.first.c_str()),
					Config::get()->getBool(setting.first),
					response.GetAllocator()
				);
			}

			else {
				configVals.AddMember(
					rapidjson::StringRef(setting.first.c_str()),
					rapidjson::StringRef(setting.second.c_str()),
					response.GetAllocator()
				);
			}
		}
	}

	response.AddMember("status", Response::STATUS_SUCCESS, response.GetAllocator());
	response.AddMember("config", configVals, response.GetAllocator());

	return response;
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
		response.AddMember("message", rapidjson::StringRef(Response::STATE_DISABLED), response.GetAllocator());
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

/*****************************************************************************/

rapidjson::Document GlobalController::restore(const rapidjson::Document &request) {

	rapidjson::Document response(rapidjson::kObjectType);

	if (!Config::get()->getBool(Config::CONFIG_KEY_STATE_ENABLED)) {
		response.AddMember("status", Response::STATUS_UNSUPPORTED, response.GetAllocator());
		response.AddMember("message", rapidjson::StringRef(Response::STATE_DISABLED), response.GetAllocator());
	}

	else {

		try {
			response.AddMember("status", GameContainer::get()->restore(), response.GetAllocator());
		}

		catch (const std::exception &e) {
			response.AddMember("status", Response::STATUS_INTERNAL_ERROR, response.GetAllocator());
			response.AddMember("message", rapidjson::StringRef(e.what()), response.GetAllocator());
		}
	}

	return response;
}
