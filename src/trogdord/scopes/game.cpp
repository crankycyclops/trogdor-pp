#include <utility>
#include <iomanip>
#include <fstream>

#include <trogdor/utility.h>
#include <trogdor/filesystem.h>

#include "../include/request.h"
#include "../include/gamecontainer.h"
#include "../include/filesystem.h"
#include "../include/serial/drivermap.h"

#include "../include/scopes/game.h"
#include "../include/exception/gameslotnotfound.h"


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

	registerAction(Request::GET, DUMPED_ACTION, [&] (const rapidjson::Document &request) -> rapidjson::Document {
		return this->getDumped(request);
	});

	registerAction(Request::POST, DEFAULT_ACTION, [&] (const rapidjson::Document &request) -> rapidjson::Document {
		return this->createGame(request);
	});

	registerAction(Request::POST, DUMP_GAME_ACTION, [&] (const rapidjson::Document &request) -> rapidjson::Document {
		return this->dumpGame(request);
	});

	registerAction(Request::POST, RESTORE_GAME_ACTION, [&] (const rapidjson::Document &request) -> rapidjson::Document {
		return this->restoreGame(request);
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

		rapidjson::Value name(rapidjson::kStringType);
		name.SetString(rapidjson::StringRef(game->getName().c_str()), response.GetAllocator());

		rapidjson::Value definition(rapidjson::kStringType);
		definition.SetString(rapidjson::StringRef(game->getDefinition().c_str()), response.GetAllocator());

		response.AddMember("status", Response::STATUS_SUCCESS, response.GetAllocator());
		response.AddMember("id", gameId, response.GetAllocator());
		response.AddMember("name", name.Move(), response.GetAllocator());
		response.AddMember("definition", definition.Move(), response.GetAllocator());
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

	if (filtersArg) {

		// If we get a null argument, I think that could be reasonably
		// construed as intending not to pass in any filters, and since
		// Filter::JSONToFilterUnion() will just pass over it anyway, I'll
		// consider it a legal value.
		if (filtersArg->IsNull() || filtersArg->IsArray() || filtersArg->IsObject()) {
			filters = Filter::JSONToFilterUnion(*filtersArg);
		}

		else {

			response.AddMember("status", Response::STATUS_INVALID, response.GetAllocator());
			response.AddMember("message", rapidjson::StringRef(INVALID_FILTER_ARG), response.GetAllocator());

			return response;
		}
	}

	for (const auto &gameId: GameContainer::get()->getGames(filters)) {

		rapidjson::Value gameJSON(rapidjson::kObjectType);
		rapidjson::Value name(rapidjson::kStringType);

		name.SetString(
			rapidjson::StringRef(GameContainer::get()->getGame(gameId)->getName().c_str()),
			response.GetAllocator()
		);

		gameJSON.AddMember("id", gameId, response.GetAllocator());
		gameJSON.AddMember("name", name.Move(), response.GetAllocator());

		// If an include_meta argument is included, it specifies Game
		// meta data values that should be included along with the game's
		// ID and name in the returned list.
		for (const auto &key: metaKeys) {

			std::string value = GameContainer::get()->getGame(gameId)->get()->getMeta(key);
			rapidjson::Value metaVal(rapidjson::kStringType);

			metaVal.SetString(value.c_str(), response.GetAllocator());
			gameJSON.AddMember(rapidjson::StringRef(key.c_str()), metaVal.Move(), response.GetAllocator());
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
		rapidjson::Value timeCreatedVal(rapidjson::kStringType);

		timeCreatedVal.SetString(timeCreated.str().c_str(), response.GetAllocator());

		response.AddMember("status", Response::STATUS_SUCCESS, response.GetAllocator());
		response.AddMember("created", timeCreatedVal, response.GetAllocator());
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
			size_t gameId = GameContainer::get()->createGame(definitionStr, nameStr, meta);
			response.AddMember("status", Response::STATUS_SUCCESS, response.GetAllocator());
			response.AddMember("id", gameId, response.GetAllocator());
		}

		catch (const ServerException &e) {

			rapidjson::Value errorMsg(rapidjson::kStringType);

			errorMsg.SetString(rapidjson::StringRef(e.what()), response.GetAllocator());
			response.AddMember("status", Response::STATUS_INTERNAL_ERROR, response.GetAllocator());
			response.AddMember("message", errorMsg, response.GetAllocator());
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

	// By default, we destroy a game's dumps along with the game itself
	bool destroyDump = true;
	const rapidjson::Value *destroyDumpVal = rapidjson::Pointer("/args/delete_dump").Get(request);

	if (destroyDumpVal) {

		if (destroyDumpVal->IsBool()) {
			destroyDump = destroyDumpVal->GetBool();
		}

		else {

			response.AddMember("status", Response::STATUS_INVALID, response.GetAllocator());
			response.AddMember("message", rapidjson::StringRef(INVALID_DELETE_DUMP_ARG), response.GetAllocator());

			return response;
		}
	}

	if (GameContainer::get()->getGame(gameId)) {
		GameContainer::get()->destroyGame(gameId, destroyDump);
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
		if (metaKeys && metaKeys->IsArray() && metaKeys->Size()) {

			for (auto i = metaKeys->Begin(); i != metaKeys->End(); i++) {

				if (i->IsString()) {

					const char *key = i->GetString();
					rapidjson::Value value(
						GameContainer::get()->getMeta(gameId, key).c_str(),
						response.GetAllocator()
					);

					meta.AddMember(rapidjson::StringRef(key), value, response.GetAllocator());
				}

				else {

					response.AddMember("status", Response::STATUS_INVALID, response.GetAllocator());
					response.AddMember("message", rapidjson::StringRef(INVALID_META_KEYS), response.GetAllocator());

					return response;
				}
			}
		}

		else if (metaKeys && !metaKeys->IsArray()) {

			response.AddMember("status", Response::STATUS_INVALID, response.GetAllocator());
			response.AddMember("message", rapidjson::StringRef(INVALID_META_KEYS), response.GetAllocator());

			return response;
		}

		// Client is requesting all currently set meta values
		else {
			for (auto &metaVal: GameContainer::get()->getMetaAll(gameId)) {

				rapidjson::Value key(rapidjson::StringRef(metaVal.first.c_str()), response.GetAllocator());
				rapidjson::Value value(rapidjson::StringRef(metaVal.second.c_str()), response.GetAllocator());

				meta.AddMember(key, value.Move(), response.GetAllocator());
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

		if (meta && meta->IsObject() && meta->MemberBegin() != meta->MemberEnd()) {

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

		else if (meta && !meta->IsObject()) {

			response.AddMember("status", Response::STATUS_INVALID, response.GetAllocator());
			response.AddMember("message", rapidjson::StringRef(INVALID_META), response.GetAllocator());

			return response;
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

/*****************************************************************************/

rapidjson::Document GameController::getDumped(const rapidjson::Document &request) {

	rapidjson::Document response(rapidjson::kObjectType);

	if (!Config::get()->getBool(Config::CONFIG_KEY_STATE_ENABLED)) {

		response.AddMember("status", Response::STATUS_UNSUPPORTED, response.GetAllocator());
		response.AddMember("message", rapidjson::StringRef(Response::STATE_DISABLED), response.GetAllocator());

		return response;
	}

	try {

		const rapidjson::Value *idArg = rapidjson::Pointer("/args/id").Get(request);

		if (!idArg) {

			rapidjson::Value jsonIds(rapidjson::kArrayType);
			std::vector<size_t> ids = GameContainer::get()->getDumpedGameIds();

			for (const auto &id: ids) {

				if (GameContainer::get()->isDumpedGameId(id)) {

					std::string gameMetaPath = Config::get()->getStatePath() +
						STD_FILESYSTEM::path::preferred_separator + std::to_string(id) +
						STD_FILESYSTEM::path::preferred_separator + "meta";

					try {

						auto &serialDriver =
							serial::DriverMap::get(Config::get()->getString(Config::CONFIG_KEY_STATE_FORMAT));

						std::ifstream metaFile(gameMetaPath);
						std::string metaStr(
							(std::istreambuf_iterator<char>(metaFile)),
							std::istreambuf_iterator<char>()
						);

						std::shared_ptr<trogdor::serial::Serializable> metaData =
							serialDriver->deserialize(metaStr);

						rapidjson::Value jsonData(rapidjson::kObjectType);
						rapidjson::Value gName(rapidjson::kStringType);
						rapidjson::Value gDefinition(rapidjson::kStringType);

						gName.SetString(rapidjson::StringRef(
							std::get<std::string>(*metaData->get("name")).c_str()
						), response.GetAllocator());
						gDefinition.SetString(rapidjson::StringRef(
							std::get<std::string>(*metaData->get("definition")).c_str()
						), response.GetAllocator());

						jsonData.AddMember("id", id, response.GetAllocator());
						jsonData.AddMember("name", gName, response.GetAllocator());
						jsonData.AddMember("definition", gDefinition, response.GetAllocator());
						jsonData.AddMember("created", std::get<size_t>(*metaData->get("created")), response.GetAllocator());

						jsonIds.PushBack(jsonData.Move(), response.GetAllocator());
					}

					// If we have a problem opening the meta file or
					// deserializing it to get the necessary data, consider
					// the dumped game invalid and ignore it.
					catch (const std::exception &e) {}
				}
			}

			response.AddMember("status", Response::STATUS_SUCCESS, response.GetAllocator());
			response.AddMember("games", jsonIds.Move(), response.GetAllocator());
		}

		else {

			#if SIZE_MAX == UINT64_MAX
				if (!idArg->IsUint64()) {
			#else
				if (!idArg->IsUint()) {
			#endif
				response.AddMember("status", Response::STATUS_INVALID, response.GetAllocator());
				response.AddMember("message", rapidjson::StringRef(Request::INVALID_GAME_ID), response.GetAllocator());
			}

			#if SIZE_MAX == UINT64_MAX
				else if (!GameContainer::get()->isDumpedGameId(idArg->GetUint64())) {
			#else
				else if (!GameContainer::get()->isDumpedGameId(idArg->GetUint())) {
			#endif
				response.AddMember("status", Response::STATUS_NOT_FOUND, response.GetAllocator());
				response.AddMember("message", rapidjson::StringRef(DUMPED_GAME_NOT_FOUND), response.GetAllocator());
			}

			else {

				std::set<size_t> slots;
				std::unordered_map<size_t, size_t> timestamps;

				rapidjson::Value slotsArr(rapidjson::kArrayType);
				std::string gamePath = Config::get()->getStatePath() +
					STD_FILESYSTEM::path::preferred_separator + std::to_string(idArg->GetUint());

				GameWrapper::getDumpedGameSlots(slots, gamePath, &timestamps);

				for (const auto &slot: slots) {

					rapidjson::Value slotData(rapidjson::kObjectType);

					slotData.AddMember("slot", slot, response.GetAllocator());
					slotData.AddMember("timestamp_ms", timestamps[slot], response.GetAllocator());
					slotsArr.PushBack(slotData, response.GetAllocator());
				}

				response.AddMember("status", Response::STATUS_SUCCESS, response.GetAllocator());
				response.AddMember("slots", slotsArr.Move(), response.GetAllocator());
			}
		}
	}

	catch (const std::exception &e) {

		rapidjson::Value error(rapidjson::kStringType);
		error.SetString(e.what(), response.GetAllocator());

		response.AddMember("status", Response::STATUS_INTERNAL_ERROR, response.GetAllocator());
		response.AddMember("message", error.Move(), response.GetAllocator());
	}

	return response;
}

/*****************************************************************************/

rapidjson::Document GameController::dumpGame(const rapidjson::Document &request) {

	size_t gameId;
	rapidjson::Document response(rapidjson::kObjectType);

	if (!Config::get()->getBool(Config::CONFIG_KEY_STATE_ENABLED)) {

		response.AddMember("status", Response::STATUS_UNSUPPORTED, response.GetAllocator());
		response.AddMember("message", rapidjson::StringRef(Response::STATE_DISABLED), response.GetAllocator());

		return response;
	}

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

		try {
			game->dump();
			response.AddMember("status", Response::STATUS_SUCCESS, response.GetAllocator());
		}

		catch (const std::exception &e) {

			rapidjson::Value error(rapidjson::kStringType);
			error.SetString(e.what(), response.GetAllocator());

			response.AddMember("status", Response::STATUS_INTERNAL_ERROR, response.GetAllocator());
			response.AddMember("message", error.Move(), response.GetAllocator());
		}
	}

	else {
		response.AddMember("status", Response::STATUS_NOT_FOUND, response.GetAllocator());
		response.AddMember("message", rapidjson::StringRef(GAME_NOT_FOUND), response.GetAllocator());
	}

	return response;
}

/*****************************************************************************/

rapidjson::Document GameController::restoreGame(const rapidjson::Document &request) {

	rapidjson::Document response(rapidjson::kObjectType);

	if (!Config::get()->getBool(Config::CONFIG_KEY_STATE_ENABLED)) {

		response.AddMember("status", Response::STATUS_UNSUPPORTED, response.GetAllocator());
		response.AddMember("message", rapidjson::StringRef(Response::STATE_DISABLED), response.GetAllocator());

		return response;
	}

	size_t gameId;
	std::optional<size_t> slot;

	try {
		gameId = Request::parseGameId(request);
	}

	catch (const rapidjson::Document &error) {
		rapidjson::Document errorCopy;
		errorCopy.CopyFrom(error, errorCopy.GetAllocator());
		return errorCopy;
	}

	const rapidjson::Value *slotArg = rapidjson::Pointer("/args/slot").Get(request);

	if (slotArg) {

		#if SIZE_MAX == UINT64_MAX
			if (!slotArg->IsUint64()) {
		#else
			if (!slotArg->IsUInt()) {
		#endif

			response.AddMember("status", Response::STATUS_INVALID, response.GetAllocator());
			response.AddMember("message", rapidjson::StringRef(INVALID_DUMPED_GAME_SLOT), response.GetAllocator());

			return response;
		}

		else {
			#if SIZE_MAX == UINT64_MAX
				slot = slotArg->GetUint64();
			#else
				slot = slotArg->GetUint();
			#endif
		}
	}

	try {

		GameContainer::get()->restoreGame(gameId, slot);

		std::string logMsg = "Restored game id " + std::to_string(gameId);
		if (slot) {
			logMsg += " (slot " + std::to_string(*slot) + ")";
		}

		Config::get()->err(trogdor::Trogerr::INFO) << logMsg << '.' << std::endl;
		response.AddMember("status", Response::STATUS_SUCCESS, response.GetAllocator());
	}

	catch (const GameNotFound &e) {
		response.AddMember("status", Response::STATUS_NOT_FOUND, response.GetAllocator());
		response.AddMember("message", rapidjson::StringRef(DUMPED_GAME_NOT_FOUND), response.GetAllocator());
	}

	catch (const GameSlotNotFound &e) {
		response.AddMember("status", Response::STATUS_NOT_FOUND, response.GetAllocator());
		response.AddMember("message", rapidjson::StringRef(DUMPED_GAME_SLOT_NOT_FOUND), response.GetAllocator());
	}

	catch (const std::exception &e) {
		Config::get()->err(trogdor::Trogerr::ERROR) << e.what() << std::endl;
		response.AddMember("status", Response::STATUS_INTERNAL_ERROR, response.GetAllocator());
		response.AddMember("message", rapidjson::StringRef(Response::INTERNAL_ERROR_MSG), response.GetAllocator());
	}

	return response;
}
