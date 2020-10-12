#include <utility>
#include <iomanip>

#include <trogdor/utility.h>
#include <trogdor/filesystem.h>

#include "../include/request.h"
#include "../include/gamecontainer.h"
#include "../include/filesystem.h"

#include "../include/scopes/game.h"

// Scope name that should be used in requests
const char *GameController::SCOPE = "game";

// Actions served by the "game" scope
const char *GameController::STATISTICS_ACTION = "statistics";
const char *GameController::LIST_ACTION = "list";
const char *GameController::META_ACTION = "meta";
const char *GameController::DEFINITIONS_ACTION = "definitions";
const char *GameController::START_ACTION = "start";
const char *GameController::STOP_ACTION = "stop";
const char *GameController::TIME_ACTION = "time";
const char *GameController::IS_RUNNING_ACTION = "is_running";

// Error messages
const char *GameController::INVALID_NAME = "game name must be a string";
const char *GameController::MISSING_REQUIRED_NAME = "missing required name";
const char *GameController::INVALID_DEFINITION = "game definition filename must be a string";
const char *GameController::MISSING_REQUIRED_DEFINITION = "missing required definition path";
const char *GameController::DEFINITION_NOT_RELATIVE = "definition path must be relative";
const char *GameController::MISSING_META = "missing required meta key, value pairs";
const char *GameController::INVALID_META = "meta values cannot be objects or arrays";
const char *GameController::INVALID_META_KEYS = "invalid meta keys";

// Singleton instance of GameController
std::unique_ptr<GameController> GameController::instance;

/*****************************************************************************/

GameController::GameController() {

	registerAction(Request::GET, DEFAULT_ACTION, [&] (const rapidjson::Document &request) -> rapidjson::Document {
		return this->getGame(request);
	});

	registerAction(Request::GET, LIST_ACTION, [&] (const rapidjson::Document &request) -> rapidjson::Document {
		return this->getGameList(request);
	});

	registerAction(Request::GET, META_ACTION, [&] (const rapidjson::Document &request) -> rapidjson::Document {
		return this->getMeta(request);
	});

	registerAction(Request::GET, STATISTICS_ACTION, [&] (const rapidjson::Document &request) -> rapidjson::Document {
		return this->getStatistics(request);
	});

	registerAction(Request::GET, TIME_ACTION, [&] (const rapidjson::Document &request) -> rapidjson::Document {
		return this->getTime(request);
	});

	registerAction(Request::GET, IS_RUNNING_ACTION, [&] (const rapidjson::Document &request) -> rapidjson::Document {
		return this->getIsRunning(request);
	});

	registerAction(Request::GET, DEFINITIONS_ACTION, [&] (const rapidjson::Document &request) -> rapidjson::Document {
		return this->getDefinitionList(request);
	});

	registerAction(Request::POST, DEFAULT_ACTION, [&] (const rapidjson::Document &request) -> rapidjson::Document {
		return this->createGame(request);
	});

	registerAction(Request::SET, META_ACTION, [&] (const rapidjson::Document &request) -> rapidjson::Document {
		return this->setMeta(request);
	});

	registerAction(Request::SET, START_ACTION, [&] (const rapidjson::Document &request) -> rapidjson::Document {
		return this->startGame(request);
	});

	registerAction(Request::SET, STOP_ACTION, [&] (const rapidjson::Document &request) -> rapidjson::Document {
		return this->stopGame(request);
	});

	registerAction(Request::DELETE, DEFAULT_ACTION, [&] (const rapidjson::Document &request) -> rapidjson::Document {
		return this->destroyGame(request);
	});
}

/*****************************************************************************/

std::unique_ptr<GameController> &GameController::get() {

	if (!instance) {
		instance = std::unique_ptr<GameController>(new GameController());
	}

	return instance;
}

/*****************************************************************************/

rapidjson::Document GameController::getGame(const rapidjson::Document &request) {

	size_t gameId;
	rapidjson::Document response(rapidjson::kObjectType);

	try {
		gameId = Request::parseGameId(request);
	}

	catch (const rapidjson::Document &error) {
		rapidjson::Document errorCopy;
		errorCopy.CopyFrom(error, errorCopy.GetAllocator());
		return errorCopy;
	}

	std::unique_ptr<GameWrapper> &game = GameContainer::get()->getGame(gameId);

	if (game) {
		response.AddMember("status", Response::STATUS_SUCCESS, response.GetAllocator());
		response.AddMember("id", gameId, response.GetAllocator());
		response.AddMember("name", rapidjson::StringRef(game->getName().c_str()), response.GetAllocator());
		response.AddMember("definition", rapidjson::StringRef(game->getDefinition().c_str()), response.GetAllocator());
		response.AddMember("current_time", game->get()->getTime(), response.GetAllocator());
		response.AddMember("is_running", game->get()->inProgress(), response.GetAllocator());
	}

	else {
		response.AddMember("status", Response::STATUS_NOT_FOUND, response.GetAllocator());
		response.AddMember("message", rapidjson::StringRef(GAME_NOT_FOUND), response.GetAllocator());
	}

	return response;
}

/*****************************************************************************/

rapidjson::Document GameController::getGameList(const rapidjson::Document &request) {

	rapidjson::Document response(rapidjson::kObjectType);
	rapidjson::Value gameList(rapidjson::kArrayType);

	std::vector<std::string> metaKeys;
	Filter::Union filters;

	const rapidjson::Value *includeMeta = rapidjson::Pointer("/args/include_meta").Get(request);
	const rapidjson::Value *filtersArg = rapidjson::Pointer("/args/filters").Get(request);;

	if (includeMeta) {

		if (includeMeta->IsArray()) {

			for (rapidjson::SizeType i = 0; i < includeMeta->Size(); i++) {

				if ((*includeMeta)[i].IsString()) {
					metaKeys.push_back((*includeMeta)[i].GetString());
				}

				else {

					response.AddMember("status", Response::STATUS_INVALID, response.GetAllocator());
					response.AddMember("message", rapidjson::StringRef(INVALID_META_KEYS), response.GetAllocator());

					return response;
				}
			}
		}

		else {

			response.AddMember("status", Response::STATUS_INVALID, response.GetAllocator());
			response.AddMember("message", rapidjson::StringRef(INVALID_META_KEYS), response.GetAllocator());

			return response;
		}
	}

	if (filtersArg && filtersArg->Size()) {
		filters = Filter::JSONToFilterUnion(*filtersArg);
	}

	for (const auto &gameId: GameContainer::get()->getGames(filters)) {

		rapidjson::Value gameJSON(rapidjson::kObjectType);

		gameJSON.AddMember("id", gameId, response.GetAllocator());
		gameJSON.AddMember("name", rapidjson::StringRef(
			GameContainer::get()->getGame(gameId)->getName().c_str()
		), response.GetAllocator());

		// If an include_meta argument is included, it specifies Game
		// meta data values that should be included along with the game's
		// ID and name in the returned list.
		for (const auto &key: metaKeys) {
			gameJSON.AddMember(rapidjson::StringRef(key.c_str()), rapidjson::StringRef(
				GameContainer::get()->getGame(gameId)->get()->getMeta(key).c_str()
			), response.GetAllocator());
		}

		gameList.PushBack(gameJSON.Move(), response.GetAllocator());
	}

	response.AddMember("status", Response::STATUS_SUCCESS, response.GetAllocator());
	response.AddMember("games", gameList.Move(), response.GetAllocator());

	return response;
}

/*****************************************************************************/

rapidjson::Document GameController::getDefinitionList(const rapidjson::Document &request) {

	rapidjson::Document response(rapidjson::kObjectType);
	rapidjson::Value definitions(rapidjson::kArrayType);

	std::string definitionsPath = Filesystem::getFullDefinitionsPath();

	try {

		for (const auto &dirEntry: STD_FILESYSTEM::recursive_directory_iterator(definitionsPath, STD_FILESYSTEM::directory_options::skip_permission_denied)) {

			std::string filename = dirEntry.path().string();

			// For now, trogdord only recognizes xml files. This will change
			// when (if) I finish implementing at least partial support for Inform 7.
			if (0 == filename.compare(filename.length() - 4, filename.length(), ".xml")) {

				rapidjson::Value definition(rapidjson::kStringType);

				filename = filename.replace(0, definitionsPath.length(), "");
				definition.SetString(rapidjson::StringRef(filename.c_str()), response.GetAllocator());
				definitions.PushBack(definition.Move(), response.GetAllocator());
			}
		}

		response.AddMember("status", Response::STATUS_SUCCESS, response.GetAllocator());
		response.AddMember("definitions", definitions.Move(), response.GetAllocator());
	}

	catch (const STD_FILESYSTEM::filesystem_error &e) {
		Config::get()->err() << e.what() << std::endl;
		response.AddMember("status", Response::STATUS_INTERNAL_ERROR, response.GetAllocator());
		response.AddMember("message", rapidjson::StringRef(Response::INTERNAL_ERROR_MSG), response.GetAllocator());
	}

	return response;
}

/*****************************************************************************/

rapidjson::Document GameController::getStatistics(const rapidjson::Document &request) {

	size_t gameId;
	rapidjson::Document response(rapidjson::kObjectType);

	try {
		gameId = Request::parseGameId(request);
	}

	catch (const rapidjson::Document &error) {
		rapidjson::Document errorCopy;
		errorCopy.CopyFrom(error, errorCopy.GetAllocator());
		return errorCopy;
	}

	std::unique_ptr<GameWrapper> &game = GameContainer::get()->getGame(gameId);

	if (game) {

		std::stringstream timeCreated;
		timeCreated << std::put_time(std::gmtime(&game->getCreated()), "%Y-%m-%d %H:%M:%S UTC");

		response.AddMember("status", Response::STATUS_SUCCESS, response.GetAllocator());
		response.AddMember("created", rapidjson::StringRef(timeCreated.str().c_str()), response.GetAllocator());
		response.AddMember("players", game->getNumPlayers(), response.GetAllocator());
		response.AddMember("current_time", game->get()->getTime(), response.GetAllocator());
		response.AddMember("is_running", game->get()->inProgress(), response.GetAllocator());
	}

	else {
		response.AddMember("status", Response::STATUS_NOT_FOUND, response.GetAllocator());
		response.AddMember("message", rapidjson::StringRef(GAME_NOT_FOUND), response.GetAllocator());
	}

	return response;
}

/*****************************************************************************/

rapidjson::Document GameController::createGame(const rapidjson::Document &request) {

	rapidjson::Document response(rapidjson::kObjectType);

	const rapidjson::Value *name = rapidjson::Pointer("/args/name").Get(request);
	const rapidjson::Value *definition = rapidjson::Pointer("/args/definition").Get(request);

	if (!name) {
		response.AddMember("status", Response::STATUS_INVALID, response.GetAllocator());
		response.AddMember("message", rapidjson::StringRef(MISSING_REQUIRED_NAME), response.GetAllocator());
	}

	else if (!name->IsString()) {
		response.AddMember("status", Response::STATUS_INVALID, response.GetAllocator());
		response.AddMember("message", rapidjson::StringRef(INVALID_NAME), response.GetAllocator());
	}

	else if (!definition) {
		response.AddMember("status", Response::STATUS_INVALID, response.GetAllocator());
		response.AddMember("message", rapidjson::StringRef(MISSING_REQUIRED_DEFINITION), response.GetAllocator());
	}

	else if (!definition->IsString()) {
		response.AddMember("status", Response::STATUS_INVALID, response.GetAllocator());
		response.AddMember("message", rapidjson::StringRef(INVALID_DEFINITION), response.GetAllocator());
	}

	else if (STD_FILESYSTEM::path(definition->GetString()).is_absolute()) {
		response.AddMember("status", Response::STATUS_INVALID, response.GetAllocator());
		response.AddMember("message", rapidjson::StringRef(DEFINITION_NOT_RELATIVE), response.GetAllocator());
	}

	else {

		std::string nameStr = name->GetString();
		std::string definitionStr = definition->GetString();

		trogdor::trim(nameStr);
		trogdor::trim(definitionStr);

		// If any custom meta data was included in the request, set it when we
		// create the game.
		std::unordered_map<std::string, std::string> meta;
		const rapidjson::Value &args = request["args"];

		for (auto i = args.MemberBegin(); i != args.MemberEnd(); i++) {

			if (i->name.IsString()) {

				std::string key = i->name.GetString();

				// Any key/value pairs that aren't the name or the definition
				// will be additional meta values that we need to set
				if (key.compare("name") && key.compare("definition")) {

					std::optional<std::string> value = JSON::valueToStr(i->value);

					if (value) {
						meta[key] = *value;
					}

					else {
						response.AddMember("status", Response::STATUS_INVALID, response.GetAllocator());
						response.AddMember("message", rapidjson::StringRef(INVALID_META), response.GetAllocator());

						return response;
					}
				}
			}

			else {

				response.AddMember("status", Response::STATUS_INVALID, response.GetAllocator());
				response.AddMember("message", rapidjson::StringRef(INVALID_META), response.GetAllocator());

				return response;
			}
		}

		try {
			response.AddMember("status", Response::STATUS_SUCCESS, response.GetAllocator());
			response.AddMember("id", GameContainer::get()->createGame(definitionStr, nameStr, meta), response.GetAllocator());
		}

		catch (const ServerException &e) {
			response.AddMember("status", Response::STATUS_INTERNAL_ERROR, response.GetAllocator());
			response.AddMember("message", rapidjson::StringRef(e.what()), response.GetAllocator());
		}
	}

	return response;
}

/*****************************************************************************/

rapidjson::Document GameController::destroyGame(const rapidjson::Document &request) {

	size_t gameId;
	rapidjson::Document response(rapidjson::kObjectType);

	try {
		gameId = Request::parseGameId(request);
	}

	catch (const rapidjson::Document &error) {
		rapidjson::Document errorCopy;
		errorCopy.CopyFrom(error, errorCopy.GetAllocator());
		return errorCopy;
	}

	if (GameContainer::get()->getGame(gameId)) {
		GameContainer::get()->destroyGame(gameId);
		response.AddMember("status", Response::STATUS_SUCCESS, response.GetAllocator());
	}

	else {
		response.AddMember("status", Response::STATUS_NOT_FOUND, response.GetAllocator());
		response.AddMember("message", rapidjson::StringRef(GAME_NOT_FOUND), response.GetAllocator());
	}

	return response;
}

/*****************************************************************************/

rapidjson::Document GameController::startGame(const rapidjson::Document &request) {

	size_t gameId;
	rapidjson::Document response(rapidjson::kObjectType);

	try {
		gameId = Request::parseGameId(request);
	}

	catch (const rapidjson::Document &error) {
		rapidjson::Document errorCopy;
		errorCopy.CopyFrom(error, errorCopy.GetAllocator());
		return errorCopy;
	}

	if (GameContainer::get()->getGame(gameId)) {
		GameContainer::get()->startGame(gameId);
		response.AddMember("status", Response::STATUS_SUCCESS, response.GetAllocator());
	}

	else {
		response.AddMember("status", Response::STATUS_NOT_FOUND, response.GetAllocator());
		response.AddMember("message", rapidjson::StringRef(GAME_NOT_FOUND), response.GetAllocator());
	}

	return response;
}

/*****************************************************************************/

rapidjson::Document GameController::stopGame(const rapidjson::Document &request) {

	size_t gameId;
	rapidjson::Document response(rapidjson::kObjectType);

	try {
		gameId = Request::parseGameId(request);
	}

	catch (const rapidjson::Document &error) {
		rapidjson::Document errorCopy;
		errorCopy.CopyFrom(error, errorCopy.GetAllocator());
		return errorCopy;
	}

	if (GameContainer::get()->getGame(gameId)) {
		GameContainer::get()->stopGame(gameId);
		response.AddMember("status", Response::STATUS_SUCCESS, response.GetAllocator());
	}

	else {
		response.AddMember("status", Response::STATUS_NOT_FOUND, response.GetAllocator());
		response.AddMember("message", rapidjson::StringRef(GAME_NOT_FOUND), response.GetAllocator());
	}

	return response;
}

/*****************************************************************************/

rapidjson::Document GameController::getMeta(const rapidjson::Document &request) {

	size_t gameId;
	rapidjson::Document response(rapidjson::kObjectType);

	try {
		gameId = Request::parseGameId(request);
	}

	catch (const rapidjson::Document &error) {
		rapidjson::Document errorCopy;
		errorCopy.CopyFrom(error, errorCopy.GetAllocator());
		return errorCopy;
	}

	if (!GameContainer::get()->getGame(gameId)) {
		response.AddMember("status", Response::STATUS_NOT_FOUND, response.GetAllocator());
		response.AddMember("message", rapidjson::StringRef(GAME_NOT_FOUND), response.GetAllocator());
	}

	else {

		rapidjson::Value meta(rapidjson::kObjectType);
		const rapidjson::Value *metaKeys = rapidjson::Pointer("/args/meta").Get(request);

		// Client is only requesting a certain set of meta values
		if (metaKeys && metaKeys->Size()) {

			for (auto i = metaKeys->MemberBegin(); i != metaKeys->MemberEnd(); i++) {

				if (i->name.IsString()) {

					std::string key = i->name.GetString();

					meta.AddMember(
						rapidjson::StringRef(key.c_str()),
						rapidjson::StringRef(GameContainer::get()->getMeta(gameId, key).c_str()),
						response.GetAllocator()
					);
				}

				else {

					response.AddMember("status", Response::STATUS_INVALID, response.GetAllocator());
					response.AddMember("message", rapidjson::StringRef(INVALID_META_KEYS), response.GetAllocator());

					return response;
				}
			}
		}

		// Client is requesting all currently set meta values
		else {
			for (auto &metaVal: GameContainer::get()->getMetaAll(gameId)) {
				meta.AddMember(
					rapidjson::StringRef(metaVal.first.c_str()),
					rapidjson::StringRef(metaVal.second.c_str()),
					response.GetAllocator()
				);
			}
		}

		response.AddMember("status", Response::STATUS_SUCCESS, response.GetAllocator());
		response.AddMember("meta", meta.Move(), response.GetAllocator());
	}

	return response;
}

/*****************************************************************************/

rapidjson::Document GameController::setMeta(const rapidjson::Document &request) {

	size_t gameId;
	rapidjson::Document response(rapidjson::kObjectType);

	try {
		gameId = Request::parseGameId(request);
	}

	catch (const rapidjson::Document &error) {
		rapidjson::Document errorCopy;
		errorCopy.CopyFrom(error, errorCopy.GetAllocator());
		return errorCopy;
	}

	if (!GameContainer::get()->getGame(gameId)) {
		response.AddMember("status", Response::STATUS_NOT_FOUND, response.GetAllocator());
		response.AddMember("message", rapidjson::StringRef(GAME_NOT_FOUND), response.GetAllocator());
	}

	else {

		const rapidjson::Value *meta = rapidjson::Pointer("/args/meta").Get(request);

		if (meta && meta->Size()) {

			for (auto i = meta->MemberBegin(); i != meta->MemberEnd(); i++) {

				if (i->name.IsString()) {

					std::string key = i->name.GetString();
					std::optional<std::string> value = JSON::valueToStr(i->value);

					if (value) {
						GameContainer::get()->setMeta(gameId, key, *value);
					}

					else {

						response.AddMember("status", Response::STATUS_INVALID, response.GetAllocator());
						response.AddMember("message", rapidjson::StringRef(INVALID_META), response.GetAllocator());

						return response;
					}
				}

				else {

					response.AddMember("status", Response::STATUS_INVALID, response.GetAllocator());
					response.AddMember("message", rapidjson::StringRef(INVALID_META_KEYS), response.GetAllocator());

					return response;
				}
			}

			response.AddMember("status", Response::STATUS_SUCCESS, response.GetAllocator());
		}

		else {
			response.AddMember("status", Response::STATUS_INVALID, response.GetAllocator());
			response.AddMember("message", rapidjson::StringRef(MISSING_META), response.GetAllocator());
		}
	}

	return response;
}

/*****************************************************************************/

rapidjson::Document GameController::getTime(const rapidjson::Document &request) {

	size_t gameId;
	rapidjson::Document response(rapidjson::kObjectType);

	try {
		gameId = Request::parseGameId(request);
	}

	catch (const rapidjson::Document &error) {
		rapidjson::Document errorCopy;
		errorCopy.CopyFrom(error, errorCopy.GetAllocator());
		return errorCopy;
	}

	std::unique_ptr<GameWrapper> &game = GameContainer::get()->getGame(gameId);

	if (game) {
		response.AddMember("status", Response::STATUS_SUCCESS, response.GetAllocator());
		response.AddMember("current_time", game->get()->getTime(), response.GetAllocator());
	} else {
		response.AddMember("status", Response::STATUS_NOT_FOUND, response.GetAllocator());
		response.AddMember("message", rapidjson::StringRef(GAME_NOT_FOUND), response.GetAllocator());
	}

	return response;
}

/*****************************************************************************/

rapidjson::Document GameController::getIsRunning(const rapidjson::Document &request) {

	size_t gameId;
	rapidjson::Document response(rapidjson::kObjectType);

	try {
		gameId = Request::parseGameId(request);
	}

	catch (const rapidjson::Document &error) {
		rapidjson::Document errorCopy;
		errorCopy.CopyFrom(error, errorCopy.GetAllocator());
		return errorCopy;
	}

	std::unique_ptr<GameWrapper> &game = GameContainer::get()->getGame(gameId);

	if (game) {
		response.AddMember("status", Response::STATUS_SUCCESS, response.GetAllocator());
		response.AddMember("is_running", game->get()->inProgress(), response.GetAllocator());
	} else {
		response.AddMember("status", Response::STATUS_NOT_FOUND, response.GetAllocator());
		response.AddMember("message", rapidjson::StringRef(GAME_NOT_FOUND), response.GetAllocator());
	}

	return response;
}
