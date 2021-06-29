#include <rapidjson/pointer.h>
#include <trogdor/entities/entity.h>

#include <trogdord/request.h>
#include <trogdord/gamecontainer.h>
#include <trogdord/io/output/driver.h>

#include <trogdord/scopes/entity.h>
#include <trogdord/exception/unsupportedoperation.h>
#include <trogdord/exception/entity/entitynotfound.h>


// Singleton instance of EntityController
std::unique_ptr<EntityController> EntityController::instance;

/*****************************************************************************/

EntityController::~EntityController() {}

/*****************************************************************************/

rapidjson::Value EntityController::entityToJSONObject(
	trogdor::entity::Entity *ePtr,
	rapidjson::MemoryPoolAllocator<> &allocator
) {

	rapidjson::Value entity(rapidjson::kObjectType);
	rapidjson::Value eName(rapidjson::kStringType);
	rapidjson::Value eType(rapidjson::kStringType);

	eName.SetString(rapidjson::StringRef(ePtr->getName().c_str()), allocator);
	eType.SetString(rapidjson::StringRef(ePtr->getTypeName().c_str()), allocator);

	entity.AddMember("name", eName, allocator);
	entity.AddMember("type", eType, allocator);

	return entity;
}

/*****************************************************************************/

trogdor::entity::Entity *EntityController::getEntityPtr(
	std::unique_ptr<trogdor::Game> &game,
	std::string entityName
) {

	trogdor::entity::Entity *ePtr = game->getEntity(entityName).get();

	if (!ePtr) {
		throw EntityNotFound();
	}

	return ePtr;
}

/*****************************************************************************/

rapidjson::Document EntityController::getEntityHelper(
	const rapidjson::Document &request,
	size_t &gameId,
	std::string &entityName,
	trogdor::entity::Entity *&ePtr,
	std::string missingNameMsg,
	std::string invalidNameMsg,
	std::string notFoundMsg
) {

	try {
		gameId = Request::parseGameId(request, "/args/game_id");
		entityName = Request::parseArgumentString(
			request,
			"/args/name",
			missingNameMsg,
			invalidNameMsg
		);
	}

	catch (const rapidjson::Document &error) {
		rapidjson::Document errorCopy;
		errorCopy.CopyFrom(error, errorCopy.GetAllocator());
		return errorCopy;
	}

	rapidjson::Document response(rapidjson::kObjectType);
	std::unique_ptr<GameWrapper> &game = GameContainer::get()->getGame(gameId);

	if (!game) {
		response.AddMember("status", Response::STATUS_NOT_FOUND, response.GetAllocator());
		response.AddMember("message", rapidjson::StringRef(GAME_NOT_FOUND), response.GetAllocator());
	}

	else {

		try {
			ePtr = getEntityPtr(game->get(), entityName);
		}

		catch (const EntityNotFound &e) {

			rapidjson::Value errorMsg(rapidjson::kStringType);

			errorMsg.SetString(notFoundMsg.c_str(), response.GetAllocator());
			response.AddMember("status", Response::STATUS_NOT_FOUND, response.GetAllocator());
			response.AddMember("message", errorMsg, response.GetAllocator());
		}
	}

	return response;
}

/*****************************************************************************/

std::vector<trogdor::entity::Entity *> EntityController::getEntityPtrList(
	std::unique_ptr<trogdor::Game> &game
) {

	std::vector<trogdor::entity::Entity *> entities;

	for (const auto &entity: game->getEntities()) {
		entities.push_back(entity.second.get());
	}

	return entities;
}

/*****************************************************************************/

EntityController::EntityController() {

	registerAction(Request::GET, DEFAULT_ACTION, [&] (const rapidjson::Document &request) -> rapidjson::Document {
		return this->getEntity(request);
	});

	registerAction(Request::GET, LIST_ACTION, [&] (const rapidjson::Document &request) -> rapidjson::Document {
		return this->getEntityList(request);
	});

	registerAction(Request::GET, OUTPUT_ACTION, [&] (const rapidjson::Document &request) -> rapidjson::Document {
		return this->getOutput(request);
	});

	registerAction(Request::POST, OUTPUT_ACTION, [&] (const rapidjson::Document &request) -> rapidjson::Document {
		return this->appendOutput(request);
	});
}

/*****************************************************************************/

std::unique_ptr<EntityController> &EntityController::get() {

	if (!instance) {
		instance = std::unique_ptr<EntityController>(new EntityController());
	}

	return instance;
}

/*****************************************************************************/

const char *EntityController::getName() {

	return SCOPE;
}

/*****************************************************************************/

rapidjson::Document EntityController::getEntity(const rapidjson::Document &request) {

	size_t gameId;
	std::string entityName;
	trogdor::entity::Entity *ePtr;

	rapidjson::Document error = getEntityHelper(
		request,
		gameId,
		entityName,
		ePtr
	);

	if (error.MemberBegin() != error.MemberEnd()) {
		return error;
	}

	else {

		rapidjson::Document response(rapidjson::kObjectType);

		response.AddMember("status", Response::STATUS_SUCCESS, response.GetAllocator());
		response.AddMember("entity", entityToJSONObject(ePtr, response.GetAllocator()), response.GetAllocator());

		return response;
	}
}

/*****************************************************************************/

rapidjson::Document EntityController::getEntityList(const rapidjson::Document &request) {

	size_t gameId;

	rapidjson::Document response(rapidjson::kObjectType);

	try {
		gameId = Request::parseGameId(request, "/args/game_id");
	}

	catch (const rapidjson::Document &error) {
		rapidjson::Document errorCopy;
		errorCopy.CopyFrom(error, errorCopy.GetAllocator());
		return errorCopy;
	}

	std::unique_ptr<GameWrapper> &game = GameContainer::get()->getGame(gameId);

	if (game) {

		rapidjson::Value entities(rapidjson::kArrayType);

		for (const auto &entity: getEntityPtrList(game->get())) {
			entities.PushBack(entityToJSONObject(entity, response.GetAllocator()), response.GetAllocator());
		}

		response.AddMember("status", Response::STATUS_SUCCESS, response.GetAllocator());
		response.AddMember("entities", entities, response.GetAllocator());
	}

	else {
		response.AddMember("status", Response::STATUS_NOT_FOUND, response.GetAllocator());
		response.AddMember("message", rapidjson::StringRef(GAME_NOT_FOUND), response.GetAllocator());
	}

	return response;
}

/*****************************************************************************/

rapidjson::Document EntityController::getOutput(const rapidjson::Document &request) {

	rapidjson::Document response(rapidjson::kObjectType);

	size_t gameId;
	trogdor::entity::Entity *ePtr;

	std::string entityName;
	std::string channel;

	output::Driver *outBuffer = output::Driver::get(
		Config::get()->getString(Config::CONFIG_KEY_OUTPUT_DRIVER)
	);

	try {
		channel = Request::parseArgumentString(
			request,
			"/args/channel",
			MISSING_CHANNEL,
			INVALID_CHANNEL
		);
	}

	catch (const rapidjson::Document &error) {
		rapidjson::Document errorCopy;
		errorCopy.CopyFrom(error, errorCopy.GetAllocator());
		return errorCopy;
	}

	rapidjson::Document error = getEntityHelper(
		request,
		gameId,
		entityName,
		ePtr
	);

	if (error.MemberBegin() != error.MemberEnd()) {
		return error;
	}

	try {

		rapidjson::Value messages(rapidjson::kArrayType);

		for (
			std::optional<output::Message> m = outBuffer->pop(gameId, entityName, channel);
			m.has_value();
			m = outBuffer->pop(gameId, entityName, channel)
		) {
			messages.PushBack(m->toJSONObject(response.GetAllocator()), response.GetAllocator());
		};

		response.AddMember("status", Response::STATUS_SUCCESS, response.GetAllocator());
		response.AddMember("messages", messages, response.GetAllocator());
	}

	catch (const UnsupportedOperation &e) {

		rapidjson::Value errorMsg(rapidjson::kStringType);

		errorMsg.SetString(rapidjson::StringRef(e.what()), response.GetAllocator());
		response.AddMember("status", Response::STATUS_UNSUPPORTED, response.GetAllocator());
		response.AddMember("message", errorMsg, response.GetAllocator());
	}

	return response;
}

/*****************************************************************************/

rapidjson::Document EntityController::appendOutput(const rapidjson::Document &request) {

	rapidjson::Document response(rapidjson::kObjectType);

	size_t gameId;
	trogdor::entity::Entity *ePtr;

	std::string entityName;
	std::string channel = trogdor::entity::Entity::DEFAULT_OUTPUT_CHANNEL;

	const rapidjson::Value *channelArg = rapidjson::Pointer("/args/channel").Get(request);
	const rapidjson::Value *messageArg = rapidjson::Pointer("/args/message").Get(request);

	if (channelArg) {

		if (channelArg->IsString()) {
			channel = channelArg->GetString();
		}

		else {

			response.AddMember("status", Response::STATUS_INVALID, response.GetAllocator());
			response.AddMember("message", rapidjson::StringRef(INVALID_CHANNEL), response.GetAllocator());

			return response;
		}
	}

	if (!messageArg) {

		response.AddMember("status", Response::STATUS_INVALID, response.GetAllocator());
		response.AddMember("message", rapidjson::StringRef(MISSING_OUTPUT_MESSAGE), response.GetAllocator());

		return response;
	}

	std::optional<std::string> outMessage = JSON::valueToStr(*messageArg);

	if (!outMessage) {

		response.AddMember("status", Response::STATUS_INVALID, response.GetAllocator());
		response.AddMember("message", rapidjson::StringRef(INVALID_OUTPUT_MESSAGE), response.GetAllocator());

		return response;
	}

	rapidjson::Document error = getEntityHelper(
		request,
		gameId,
		entityName,
		ePtr
	);

	if (error.MemberBegin() != error.MemberEnd()) {
		return error;
	}

	ePtr->out(channel) << *outMessage << std::endl;

	response.AddMember("status", Response::STATUS_SUCCESS, response.GetAllocator());
	return response;
}
