#include "trogdord.h"
#include "json.h"
#include "request.h"
#include "utility.h"

#include "phpexception.h"
#include "exception/requestexception.h"

#include "game.h"
#include "entities/entity.h"

using namespace rapidjson;

ZEND_DECLARE_MODULE_GLOBALS(game);
ZEND_EXTERN_MODULE_GLOBALS(trogdord);

// Exception message when methods are called on a game that's already been destroyed
const char *GAME_ALREADY_DESTROYED = "Game has already been destroyed";

// The private property that stores the game's name
const char *GAME_NAME_PROPERTY = "name";

// The private property that stores the game's definition filename
const char *GAME_DEFINITION_PROPERTY = "definition";

// The private property that stores the game's id
const char *GAME_ID_PROPERTY = "id";

// The private property through which an instance of \Trogdord\Game can access
// the connection that spawned it
const char *TROGDORD_PROPERTY = "trogdord";

// This request starts the game
static const char *GAME_START_REQUEST = "{\"method\":\"set\",\"scope\":\"game\",\"action\":\"start\",\"args\":{\"id\": %gid}}";

// This request stops the game
static const char *GAME_STOP_REQUEST = "{\"method\":\"set\",\"scope\":\"game\",\"action\":\"stop\",\"args\":{\"id\": %gid}}";

// This request destroys the game
static const char *GAME_DESTROY_REQUEST = "{\"method\":\"delete\",\"scope\":\"game\",\"args\":{\"id\": %gid%deletedumparg}}";

// This request retrieves meta data associated with the game
static const char *GAME_GET_META_REQUEST = "{\"method\":\"get\",\"scope\":\"game\",\"action\":\"meta\",\"args\":{\"id\":%gid%metaarg}}";

// This request sets meta data for the game
static const char *GAME_SET_META_REQUEST = "{\"method\":\"set\",\"scope\":\"game\",\"action\":\"meta\",\"args\":{\"id\":%gid,\"meta\":{%values}}}";

// This request returns a list of entities of the specified type
static const char *ENTITY_LIST_REQUEST = "{\"method\":\"get\",\"scope\":\"%etype\",\"action\":\"list\",\"args\":{\"game_id\":%gid}}";

// This request returns the specified entity (if it exists)
static const char *ENTITY_GET_REQUEST = "{\"method\":\"get\",\"scope\":\"%etype\",\"args\":{\"game_id\":%gid,\"name\":\"%ename\"}}";

// This request creates a new player in the specified game
static const char *NEW_PLAYER_REQUEST = "{\"method\":\"post\",\"scope\":\"player\",\"args\":{\"game_id\":%gid,\"name\":\"%pname\"}}";

// This request retrieves the current time in the given game
static const char *GAME_TIME_REQUEST = "{\"method\":\"get\",\"scope\":\"game\",\"action\":\"time\",\"args\":{\"id\":%gid}}";

// This request retrieves whether or not the specified game is running
static const char *GAME_IS_RUNNING_REQUEST = "{\"method\":\"get\",\"scope\":\"game\",\"action\":\"is_running\",\"args\":{\"id\":%gid}}";

// This request retrieves all statistics associated with a game
static const char *GAME_STATISTICS_REQUEST = "{\"method\":\"get\",\"scope\":\"game\",\"action\":\"statistics\",\"args\":{\"id\":%gid}}";

// Utility method that retrieves a list of all entities of the given type for
// the specified game. Caller must be prepared to catch NetworkException and
// RequestException.
static zval getEntityList(size_t gameId, std::string type, zval *trogdord);

// Retrieve an entity from a game and instantiate a PHP entity class that wraps
// around it. Caller must be prepared to catch NetworkException and
// RequestException.
static zval getEntity(std::string name, std::string type, zval *game);

/*****************************************************************************/

// The constructor should NEVER be called in PHP userland. Instead, instances of
// Trogdord\Game will be returned by calls to \Trogdord::getGame() and
// \Trogdord::newGame().
ZEND_BEGIN_ARG_INFO(arginfoCtor, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Game, __construct) {

	php_error_docref(NULL, E_ERROR, "Tried to instantiate non-instantiable class.");
}

/*****************************************************************************/

// Magic "__get" allows us to make private and protected data members read-only.
ZEND_BEGIN_ARG_INFO(arginfoMagicGet, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Game, __get) {

	char *key;
	size_t keyLength;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &key, &keyLength) == FAILURE) {
		RETURN_NULL();
	}

	zval rv; // ???
	zval *propVal = GAME_TO_PROP_VAL(getThis(), &rv, key);

	// There's some insanity in how this works, so for reference, here's what
	// I read to help me understand what all the arguments mean:
	// https://medium.com/@davidtstrauss/copy-and-move-semantics-of-zvals-in-php-7-41427223d784
	RETURN_ZVAL(propVal, 1, 0);
}

/*****************************************************************************/

// Starts the game. Throws an instance of \Trogdord\NetworkException if the call
// fails due to network connectivity issues. If the game has already been
// destroyed, \Trogdord\GameNotFound will be thrown.
ZEND_BEGIN_ARG_INFO(arginfoStart, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Game, start) {

	zval rv; // ???

	zval *trogdord = GAME_TO_TROGDORD(getThis(), &rv);
	zval *id = GAME_TO_ID(getThis(), &rv);

	ASSERT_GAME_ID_IS_VALID(Z_TYPE_P(id));

	try {

		std::string request = GAME_START_REQUEST;
		strReplace(request, "%gid", std::to_string(Z_LVAL_P(id)));

		trogdordObject *objWrapper = ZOBJ_TO_TROGDORD(Z_OBJ_P(trogdord));

		Document response = Request::execute(
			objWrapper->data.hostname,
			objWrapper->data.port,
			request,
			trogdord
		);
	}

	// Throw \Trogord\NetworkException
	catch (const NetworkException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(networkException), e.what(), 0);
		RETURN_NULL();
	}

	catch (const RequestException &e) {

		// Throw \Trogdord\GameNotFound
		if (404 == e.getCode()) {
			zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), e.getCode());
			RETURN_NULL();
		}

		// Throw \Trogdord\RequestException
		else {
			zend_throw_exception(EXCEPTION_GLOBALS(requestException), e.what(), e.getCode());
			RETURN_NULL();
		}
	}
}

/*****************************************************************************/

// Stops the game. Throws an instance of \Trogdord\NetworkException if the call
// fails due to network connectivity issues. If the game has already been
// destroyed, \Trogdord\GameNotFound will be thrown.
ZEND_BEGIN_ARG_INFO(arginfoStop, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Game, stop) {

	zval rv; // ???

	zval *trogdord = GAME_TO_TROGDORD(getThis(), &rv);
	zval *id = GAME_TO_ID(getThis(), &rv);

	ASSERT_GAME_ID_IS_VALID(Z_TYPE_P(id));

	try {

		std::string request = GAME_STOP_REQUEST;
		strReplace(request, "%gid", std::to_string(Z_LVAL_P(id)));

		trogdordObject *objWrapper = ZOBJ_TO_TROGDORD(Z_OBJ_P(trogdord));

		Document response = Request::execute(
			objWrapper->data.hostname,
			objWrapper->data.port,
			request,
			trogdord
		);
	}

	// Throw \Trogord\NetworkException
	catch (const NetworkException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(networkException), e.what(), 0);
		RETURN_NULL();
	}

	catch (const RequestException &e) {

		// Throw \Trogdord\GameNotFound
		if (404 == e.getCode()) {
			zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), e.getCode());
			RETURN_NULL();
		}

		// Throw \Trogdord\RequestException
		else {
			zend_throw_exception(EXCEPTION_GLOBALS(requestException), e.what(), e.getCode());
			RETURN_NULL();
		}
	}
}

/*****************************************************************************/

// Returns the game's current time. Throws an instance of
// \Trogdord\NetworkException if the call fails due to network connectivity
// issues. If the game has already been destroyed, \Trogdord\GameNotFound will
// be thrown.
ZEND_BEGIN_ARG_INFO(arginfoGetTime, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Game, getTime) {

	zval rv; // ???

	zval *trogdord = GAME_TO_TROGDORD(getThis(), &rv);
	zval *id = GAME_TO_ID(getThis(), &rv);

	ASSERT_GAME_ID_IS_VALID(Z_TYPE_P(id));

	try {

		std::string request = GAME_TIME_REQUEST;
		strReplace(request, "%gid", std::to_string(Z_LVAL_P(id)));

		trogdordObject *objWrapper = ZOBJ_TO_TROGDORD(Z_OBJ_P(trogdord));

		Document response = Request::execute(
			objWrapper->data.hostname,
			objWrapper->data.port,
			request,
			trogdord
		);

		#ifdef ZEND_ENABLE_ZVAL_LONG64
			int64_t time = response["current_time"].GetInt64();
		#else
			int time = response["current_time"].GetInt();
		#endif

		RETURN_LONG(time);
	}

	// Throw \Trogord\NetworkException
	catch (const NetworkException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(networkException), e.what(), 0);
		RETURN_NULL();
	}

	catch (const RequestException &e) {

		// Throw \Trogdord\GameNotFound
		if (404 == e.getCode()) {
			zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), e.getCode());
			RETURN_NULL();
		}

		// Throw \Trogdord\RequestException
		else {
			zend_throw_exception(EXCEPTION_GLOBALS(requestException), e.what(), e.getCode());
			RETURN_NULL();
		}
	}
}

/*****************************************************************************/

// Returns whether or not the game is currently running. Throws an instance of
// \Trogdord\NetworkException if the call fails due to network connectivity
// issues. If the game has already been destroyed, \Trogdord\GameNotFound will
// be thrown.
ZEND_BEGIN_ARG_INFO(arginfoIsRunning, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Game, isRunning) {

	zval rv; // ???

	zval *trogdord = GAME_TO_TROGDORD(getThis(), &rv);
	zval *id = GAME_TO_ID(getThis(), &rv);

	ASSERT_GAME_ID_IS_VALID(Z_TYPE_P(id));

	try {

		std::string request = GAME_IS_RUNNING_REQUEST;
		strReplace(request, "%gid", std::to_string(Z_LVAL_P(id)));

		trogdordObject *objWrapper = ZOBJ_TO_TROGDORD(Z_OBJ_P(trogdord));

		Document response = Request::execute(
			objWrapper->data.hostname,
			objWrapper->data.port,
			request,
			trogdord
		);

		RETURN_BOOL(response["is_running"].GetBool());
	}

	// Throw \Trogord\NetworkException
	catch (const NetworkException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(networkException), e.what(), 0);
		RETURN_NULL();
	}

	catch (const RequestException &e) {

		// Throw \Trogdord\GameNotFound
		if (404 == e.getCode()) {
			zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), e.getCode());
			RETURN_NULL();
		}

		// Throw \Trogdord\RequestException
		else {
			zend_throw_exception(EXCEPTION_GLOBALS(requestException), e.what(), e.getCode());
			RETURN_NULL();
		}
	}
}

/*****************************************************************************/

// Returns all statistical data associated with the game. Throws an instance of
// \Trogdord\NetworkException if the call fails due to network connectivity
// issues. If the game has already been destroyed, \Trogdord\GameNotFound will
// be thrown.
ZEND_BEGIN_ARG_INFO(arginfoStatistics, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Game, statistics) {

	zval rv; // ???

	zval *trogdord = GAME_TO_TROGDORD(getThis(), &rv);
	zval *id = GAME_TO_ID(getThis(), &rv);

	ASSERT_GAME_ID_IS_VALID(Z_TYPE_P(id));

	try {

		std::string request = GAME_STATISTICS_REQUEST;
		strReplace(request, "%gid", std::to_string(Z_LVAL_P(id)));

		trogdordObject *objWrapper = ZOBJ_TO_TROGDORD(Z_OBJ_P(trogdord));

		Document response = Request::execute(
			objWrapper->data.hostname,
			objWrapper->data.port,
			request,
			trogdord
		);

		response.RemoveMember("status");
		zval stats = JSON::JSONToZval(response);
		RETURN_ZVAL(&stats, 1, 1);
	}

	// Throw \Trogord\NetworkException
	catch (const NetworkException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(networkException), e.what(), 0);
		RETURN_NULL();
	}

	catch (const RequestException &e) {

		// Throw \Trogdord\GameNotFound
		if (404 == e.getCode()) {
			zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), e.getCode());
			RETURN_NULL();
		}

		// Throw \Trogdord\RequestException
		else {
			zend_throw_exception(EXCEPTION_GLOBALS(requestException), e.what(), e.getCode());
			RETURN_NULL();
		}
	}
}

/*****************************************************************************/

// Destroys the game, making it permanently inaccessible. Throws an instance of
// \Trogdord\NetworkException if the call fails due to network connectivity
// issues. If the game has already been destroyed, \Trogdord\GameNotFound will
// be thrown.
ZEND_BEGIN_ARG_INFO_EX(arginfoDestroy, 0, 0, 0)
	ZEND_ARG_INFO(0, deleteDump)
ZEND_END_ARG_INFO()

PHP_METHOD(Game, destroy) {

	zval rv; // ???
	zend_bool deleteDump = false;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &deleteDump) == FAILURE) {
		RETURN_NULL();
	}

	zval *trogdord = GAME_TO_TROGDORD(getThis(), &rv);
	zval *id = GAME_TO_ID(getThis(), &rv);

	ASSERT_GAME_ID_IS_VALID(Z_TYPE_P(id));

	try {

		std::string request = GAME_DESTROY_REQUEST;
		strReplace(request, "%gid", std::to_string(Z_LVAL_P(id)));

		if (ZEND_NUM_ARGS()) {
			if (deleteDump) {
				strReplace(request, "%deletedumparg", ",\"delete_dump\":true");
			} else {
				strReplace(request, "%deletedumparg", ",\"delete_dump\":false");
			}
		} else {
			strReplace(request, "%deletedumparg", "");
		}

		trogdordObject *objWrapper = ZOBJ_TO_TROGDORD(Z_OBJ_P(trogdord));

		Document response = Request::execute(
			objWrapper->data.hostname,
			objWrapper->data.port,
			request,
			trogdord
		);

		// Set the game ID to null so the object can't be used anymore
		zend_update_property_null(
			GAME_GLOBALS(classEntry),
			#if ZEND_MODULE_API_NO >= 20200930 // PHP 8.0+
				Z_OBJ_P(getThis()),
			#else
				getThis(),
			#endif
			GAME_ID_PROPERTY,
			strlen(GAME_ID_PROPERTY)
		);
	}

	// Throw \Trogord\NetworkException
	catch (const NetworkException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(networkException), e.what(), 0);
		RETURN_NULL();
	}

	catch (const RequestException &e) {

		// Throw \Trogdord\GameNotFound
		if (404 == e.getCode()) {
			zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), e.getCode());
			RETURN_NULL();
		}

		// Throw \Trogdord\RequestException
		else {
			zend_throw_exception(EXCEPTION_GLOBALS(requestException), e.what(), e.getCode());
			RETURN_NULL();
		}
	}
}

/*****************************************************************************/

// Returns meta data associated with the game. Takes an optional array argument
// (numerically indexed list of meta keys) that determines which values should
// be returned. If no argument is passed to this method, all meta data will be
// returned. Throws an instance of \Trogdord\NetworkException if the call fails
// due to network connectivity issues. If the game has already been destroyed,
// \Trogdord\GameNotFound will be thrown.
ZEND_BEGIN_ARG_INFO_EX(arginfoGetMeta, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, keys, IS_ARRAY, 1)
ZEND_END_ARG_INFO()

PHP_METHOD(Game, getMeta) {

	zval rv; // ???

	zval *trogdord = GAME_TO_TROGDORD(getThis(), &rv);
	zval *id = GAME_TO_ID(getThis(), &rv);

	ASSERT_GAME_ID_IS_VALID(Z_TYPE_P(id));

	zval *keys = nullptr;
	std::string metaArg;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|a", &keys) == FAILURE) {
		RETURN_NULL();
	}

	if (nullptr != keys && zend_array_count(Z_ARRVAL_P(keys))) {

		zval *entry;
		HashPosition pos;

		bool firstEntryVisited = false;
		metaArg = ",\"meta\":[";

		for (
			zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(keys), &pos);
			entry = zend_hash_get_current_data_ex(Z_ARRVAL_P(keys), &pos);
			zend_hash_move_forward_ex(Z_ARRVAL_P(keys), &pos)
		) {

			convert_to_string(entry);

			if (!firstEntryVisited) {
				firstEntryVisited = true;
			} else {
				metaArg += ",";
			}

			metaArg += std::string("\"") + Z_STRVAL_P(entry) + "\"";
		}

		metaArg += "]";
	}

	try {

		std::string request = GAME_GET_META_REQUEST;
		strReplace(request, "%gid", std::to_string(Z_LVAL_P(id)));
		strReplace(request, "%metaarg", metaArg);

		trogdordObject *objWrapper = ZOBJ_TO_TROGDORD(Z_OBJ_P(trogdord));

		Document response = Request::execute(
			objWrapper->data.hostname,
			objWrapper->data.port,
			request,
			trogdord
		);

		zval meta = JSON::JSONToZval(response["meta"]);
		RETURN_ZVAL(&meta, 1, 1);
	}

	// Throw \Trogord\NetworkException
	catch (const NetworkException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(networkException), e.what(), 0);
		RETURN_NULL();
	}

	catch (const RequestException &e) {

		// Throw \Trogdord\GameNotFound
		if (404 == e.getCode()) {
			zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), e.getCode());
			RETURN_NULL();
		}

		// Throw \Trogdord\RequestException
		else {
			zend_throw_exception(EXCEPTION_GLOBALS(requestException), e.what(), e.getCode());
			RETURN_NULL();
		}
	}
}

/*****************************************************************************/

ZEND_BEGIN_ARG_INFO(arginfoSetMeta, 0)
	ZEND_ARG_TYPE_INFO(0, meta, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Game, setMeta) {

	zval rv; // ???

	zval *trogdord = GAME_TO_TROGDORD(getThis(), &rv);
	zval *id = GAME_TO_ID(getThis(), &rv);

	ASSERT_GAME_ID_IS_VALID(Z_TYPE_P(id));

	zval *meta;
	std::string valuesArg;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "a", &meta) == FAILURE) {
		RETURN_NULL();
	}

	zval *entry;
	HashPosition pos;

	bool firstEntryVisited = false;

	for (
		zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(meta), &pos);
		entry = zend_hash_get_current_data_ex(Z_ARRVAL_P(meta), &pos);
		zend_hash_move_forward_ex(Z_ARRVAL_P(meta), &pos)
	) {

		zend_string *strIndex;
		zend_ulong nIndex;
		std::string key;

		switch(zend_hash_get_current_key_ex(Z_ARRVAL_P(meta), &strIndex, &nIndex, &pos)) {

			case HASH_KEY_IS_LONG:
				key = std::to_string(nIndex);
				break;

			case HASH_KEY_IS_STRING:
				key = ZSTR_VAL(strIndex);
				break;
		}

		convert_to_string(entry);

		if (!firstEntryVisited) {
			firstEntryVisited = true;
		} else {
			valuesArg += ",";
		}

		valuesArg += std::string("\"") + key + "\":\"" + Z_STRVAL_P(entry) + "\"";
	}

	try {

		std::string request = GAME_SET_META_REQUEST;
		strReplace(request, "%gid", std::to_string(Z_LVAL_P(id)));
		strReplace(request, "%values", valuesArg);

		trogdordObject *objWrapper = ZOBJ_TO_TROGDORD(Z_OBJ_P(trogdord));

		Document response = Request::execute(
			objWrapper->data.hostname,
			objWrapper->data.port,
			request,
			trogdord
		);

		RETURN_NULL();
	}

	// Throw \Trogord\NetworkException
	catch (const NetworkException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(networkException), e.what(), 0);
		RETURN_NULL();
	}

	catch (const RequestException &e) {

		// Throw \Trogdord\GameNotFound
		if (404 == e.getCode()) {
			zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), e.getCode());
			RETURN_NULL();
		}

		// Throw \Trogdord\RequestException
		else {
			zend_throw_exception(EXCEPTION_GLOBALS(requestException), e.what(), e.getCode());
			RETURN_NULL();
		}
	}
}

/*****************************************************************************/

// Returns a list of all entities in the game. Throws an instance of
// \Trogdord\NetworkException if the call fails due to network connectivity
// issues and an instance of \Trogdord\GameNotFound if the game has since been
// destroyed and no longer exists.
ZEND_BEGIN_ARG_INFO(arginfoListEntities, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Game, entities) {

	zval rv; // ???

	zval *trogdord = GAME_TO_TROGDORD(getThis(), &rv);
	zval *id = GAME_TO_ID(getThis(), &rv);

	ASSERT_GAME_ID_IS_VALID(Z_TYPE_P(id));

	try {
		zval list = getEntityList(Z_LVAL_P(id), ENTITY_TYPE_STR, trogdord);
		RETURN_ZVAL(&list, 1, 1);
	}

	// Throw \Trogord\NetworkException
	catch (const NetworkException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(networkException), e.what(), 0);
		RETURN_NULL();
	}

	catch (const RequestException &e) {

		// Throw \Trogdord\GameNotFound
		if (404 == e.getCode()) {
			zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), e.getCode());
			RETURN_NULL();
		}

		// Throw \Trogdord\RequestException
		else {
			zend_throw_exception(EXCEPTION_GLOBALS(requestException), e.what(), e.getCode());
			RETURN_NULL();
		}
	}
}

/*****************************************************************************/

// Returns a list of all tangibles in the game. Throws an instance of
// \Trogdord\NetworkException if the call fails due to network connectivity
// issues and an instance of \Trogdord\GameNotFound if the game has since been
// destroyed and no longer exists.
ZEND_BEGIN_ARG_INFO(arginfoListTangibles, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Game, tangibles) {

	zval rv; // ???

	zval *trogdord = GAME_TO_TROGDORD(getThis(), &rv);
	zval *id = GAME_TO_ID(getThis(), &rv);

	ASSERT_GAME_ID_IS_VALID(Z_TYPE_P(id));

	try {
		zval list = getEntityList(Z_LVAL_P(id), TANGIBLE_TYPE_STR, trogdord);
		RETURN_ZVAL(&list, 1, 1);
	}

	// Throw \Trogord\NetworkException
	catch (const NetworkException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(networkException), e.what(), 0);
		RETURN_NULL();
	}

	catch (const RequestException &e) {

		// Throw \Trogdord\GameNotFound
		if (404 == e.getCode()) {
			zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), e.getCode());
			RETURN_NULL();
		}

		// Throw \Trogdord\RequestException
		else {
			zend_throw_exception(EXCEPTION_GLOBALS(requestException), e.what(), e.getCode());
			RETURN_NULL();
		}
	}
}

/*****************************************************************************/

// Returns a list of all places in the game. Throws an instance of
// \Trogdord\NetworkException if the call fails due to network connectivity
// issues and an instance of \Trogdord\GameNotFound if the game has since been
// destroyed and no longer exists.
ZEND_BEGIN_ARG_INFO(arginfoListPlaces, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Game, places) {

	zval rv; // ???

	zval *trogdord = GAME_TO_TROGDORD(getThis(), &rv);
	zval *id = GAME_TO_ID(getThis(), &rv);

	ASSERT_GAME_ID_IS_VALID(Z_TYPE_P(id));

	try {
		zval list = getEntityList(Z_LVAL_P(id), PLACE_TYPE_STR, trogdord);
		RETURN_ZVAL(&list, 1, 1);
	}

	// Throw \Trogord\NetworkException
	catch (const NetworkException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(networkException), e.what(), 0);
		RETURN_NULL();
	}

	catch (const RequestException &e) {

		// Throw \Trogdord\GameNotFound
		if (404 == e.getCode()) {
			zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), e.getCode());
			RETURN_NULL();
		}

		// Throw \Trogdord\RequestException
		else {
			zend_throw_exception(EXCEPTION_GLOBALS(requestException), e.what(), e.getCode());
			RETURN_NULL();
		}
	}
}

/*****************************************************************************/

// Returns a list of all things in the game. Throws an instance of
// \Trogdord\NetworkException if the call fails due to network connectivity
// issues and an instance of \Trogdord\GameNotFound if the game has since been
// destroyed and no longer exists.
ZEND_BEGIN_ARG_INFO(arginfoListThings, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Game, things) {

	zval rv; // ???

	zval *trogdord = GAME_TO_TROGDORD(getThis(), &rv);
	zval *id = GAME_TO_ID(getThis(), &rv);

	ASSERT_GAME_ID_IS_VALID(Z_TYPE_P(id));

	try {
		zval list = getEntityList(Z_LVAL_P(id), THING_TYPE_STR, trogdord);
		RETURN_ZVAL(&list, 1, 1);
	}

	// Throw \Trogord\NetworkException
	catch (const NetworkException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(networkException), e.what(), 0);
		RETURN_NULL();
	}

	catch (const RequestException &e) {

		// Throw \Trogdord\GameNotFound
		if (404 == e.getCode()) {
			zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), e.getCode());
			RETURN_NULL();
		}

		// Throw \Trogdord\RequestException
		else {
			zend_throw_exception(EXCEPTION_GLOBALS(requestException), e.what(), e.getCode());
			RETURN_NULL();
		}
	}
}

/*****************************************************************************/

// Returns a list of all beings in the game. Throws an instance of
// \Trogdord\NetworkException if the call fails due to network connectivity
// issues and an instance of \Trogdord\GameNotFound if the game has since been
// destroyed and no longer exists.
ZEND_BEGIN_ARG_INFO(arginfoListBeings, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Game, beings) {

	zval rv; // ???

	zval *trogdord = GAME_TO_TROGDORD(getThis(), &rv);
	zval *id = GAME_TO_ID(getThis(), &rv);

	ASSERT_GAME_ID_IS_VALID(Z_TYPE_P(id));

	try {
		zval list = getEntityList(Z_LVAL_P(id), BEING_TYPE_STR, trogdord);
		RETURN_ZVAL(&list, 1, 1);
	}

	// Throw \Trogord\NetworkException
	catch (const NetworkException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(networkException), e.what(), 0);
		RETURN_NULL();
	}

	catch (const RequestException &e) {

		// Throw \Trogdord\GameNotFound
		if (404 == e.getCode()) {
			zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), e.getCode());
			RETURN_NULL();
		}

		// Throw \Trogdord\RequestException
		else {
			zend_throw_exception(EXCEPTION_GLOBALS(requestException), e.what(), e.getCode());
			RETURN_NULL();
		}
	}
}

/*****************************************************************************/

// Returns a list of all resources in the game. Throws an instance of
// \Trogdord\NetworkException if the call fails due to network connectivity
// issues and an instance of \Trogdord\GameNotFound if the game has since been
// destroyed and no longer exists.
ZEND_BEGIN_ARG_INFO(arginfoListResources, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Game, resources) {

	zval rv; // ???

	zval *trogdord = GAME_TO_TROGDORD(getThis(), &rv);
	zval *id = GAME_TO_ID(getThis(), &rv);

	ASSERT_GAME_ID_IS_VALID(Z_TYPE_P(id));

	try {
		zval list = getEntityList(Z_LVAL_P(id), RESOURCE_TYPE_STR, trogdord);
		RETURN_ZVAL(&list, 1, 1);
	}

	// Throw \Trogord\NetworkException
	catch (const NetworkException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(networkException), e.what(), 0);
		RETURN_NULL();
	}

	catch (const RequestException &e) {

		// Throw \Trogdord\GameNotFound
		if (404 == e.getCode()) {
			zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), e.getCode());
			RETURN_NULL();
		}

		// Throw \Trogdord\RequestException
		else {
			zend_throw_exception(EXCEPTION_GLOBALS(requestException), e.what(), e.getCode());
			RETURN_NULL();
		}
	}
}

/*****************************************************************************/

// Returns a list of all rooms in the game. Throws an instance of
// \Trogdord\NetworkException if the call fails due to network connectivity
// issues and an instance of \Trogdord\GameNotFound if the game has since been
// destroyed and no longer exists.
ZEND_BEGIN_ARG_INFO(arginfoListRooms, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Game, rooms) {

	zval rv; // ???

	zval *trogdord = GAME_TO_TROGDORD(getThis(), &rv);
	zval *id = GAME_TO_ID(getThis(), &rv);

	ASSERT_GAME_ID_IS_VALID(Z_TYPE_P(id));

	try {
		zval list = getEntityList(Z_LVAL_P(id), ROOM_TYPE_STR, trogdord);
		RETURN_ZVAL(&list, 1, 1);
	}

	// Throw \Trogord\NetworkException
	catch (const NetworkException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(networkException), e.what(), 0);
		RETURN_NULL();
	}

	catch (const RequestException &e) {

		// Throw \Trogdord\GameNotFound
		if (404 == e.getCode()) {
			zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), e.getCode());
			RETURN_NULL();
		}

		// Throw \Trogdord\RequestException
		else {
			zend_throw_exception(EXCEPTION_GLOBALS(requestException), e.what(), e.getCode());
			RETURN_NULL();
		}
	}
}

/*****************************************************************************/

// Returns a list of all objects in the game. Throws an instance of
// \Trogdord\NetworkException if the call fails due to network connectivity
// issues and an instance of \Trogdord\GameNotFound if the game has since been
// destroyed and no longer exists.
ZEND_BEGIN_ARG_INFO(arginfoListObjects, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Game, objects) {

	zval rv; // ???

	zval *trogdord = GAME_TO_TROGDORD(getThis(), &rv);
	zval *id = GAME_TO_ID(getThis(), &rv);

	ASSERT_GAME_ID_IS_VALID(Z_TYPE_P(id));

	try {
		zval list = getEntityList(Z_LVAL_P(id), OBJECT_TYPE_STR, trogdord);
		RETURN_ZVAL(&list, 1, 1);
	}

	// Throw \Trogord\NetworkException
	catch (const NetworkException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(networkException), e.what(), 0);
		RETURN_NULL();
	}

	catch (const RequestException &e) {

		// Throw \Trogdord\GameNotFound
		if (404 == e.getCode()) {
			zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), e.getCode());
			RETURN_NULL();
		}

		// Throw \Trogdord\RequestException
		else {
			zend_throw_exception(EXCEPTION_GLOBALS(requestException), e.what(), e.getCode());
			RETURN_NULL();
		}
	}
}

/*****************************************************************************/

// Returns a list of all creatures in the game. Throws an instance of
// \Trogdord\NetworkException if the call fails due to network connectivity
// issues and an instance of \Trogdord\GameNotFound if the game has since been
// destroyed and no longer exists.
ZEND_BEGIN_ARG_INFO(arginfoListCreatures, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Game, creatures) {

	zval rv; // ???

	zval *trogdord = GAME_TO_TROGDORD(getThis(), &rv);
	zval *id = GAME_TO_ID(getThis(), &rv);

	ASSERT_GAME_ID_IS_VALID(Z_TYPE_P(id));

	try {
		zval list = getEntityList(Z_LVAL_P(id), CREATURE_TYPE_STR, trogdord);
		RETURN_ZVAL(&list, 1, 1);
	}

	// Throw \Trogord\NetworkException
	catch (const NetworkException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(networkException), e.what(), 0);
		RETURN_NULL();
	}

	catch (const RequestException &e) {

		// Throw \Trogdord\GameNotFound
		if (404 == e.getCode()) {
			zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), e.getCode());
			RETURN_NULL();
		}

		// Throw \Trogdord\RequestException
		else {
			zend_throw_exception(EXCEPTION_GLOBALS(requestException), e.what(), e.getCode());
			RETURN_NULL();
		}
	}
}

/*****************************************************************************/

// Returns a list of all players in the game. Throws an instance of
// \Trogdord\NetworkException if the call fails due to network connectivity
// issues and an instance of \Trogdord\GameNotFound if the game has since been
// destroyed and no longer exists.
ZEND_BEGIN_ARG_INFO(arginfoListPlayers, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Game, players) {

	zval rv; // ???

	zval *trogdord = GAME_TO_TROGDORD(getThis(), &rv);
	zval *id = GAME_TO_ID(getThis(), &rv);

	ASSERT_GAME_ID_IS_VALID(Z_TYPE_P(id));

	try {
		zval list = getEntityList(Z_LVAL_P(id), PLAYER_TYPE_STR, trogdord);
		RETURN_ZVAL(&list, 1, 1);
	}

	// Throw \Trogord\NetworkException
	catch (const NetworkException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(networkException), e.what(), 0);
		RETURN_NULL();
	}

	catch (const RequestException &e) {

		// Throw \Trogdord\GameNotFound
		if (404 == e.getCode()) {
			zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), e.getCode());
			RETURN_NULL();
		}

		// Throw \Trogdord\RequestException
		else {
			zend_throw_exception(EXCEPTION_GLOBALS(requestException), e.what(), e.getCode());
			RETURN_NULL();
		}
	}
}

/*****************************************************************************/

// Returns an object that inherits from \Trogdord\Entity representing the
// specified entity in the game. Throws an instance of
// \Trogdord\NetworkException if the call fails due to network connectivity
// issues, an instance of \Trogdord\GameNotFound if the game has since been
// destroyed and no longer exists, and an instance of Trogdord\EntityNotFound if
// the specified entity does not exist.
ZEND_BEGIN_ARG_INFO(arginfoGetEntity, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Game, getEntity) {

	char *name;
	size_t nameLength;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name, &nameLength) == FAILURE) {
		RETURN_NULL();
	}

	zval rv;
	zval *gameId = GAME_TO_ID(getThis(), &rv);

	ASSERT_GAME_ID_IS_VALID(Z_TYPE_P(gameId));

	try {
		zval entity = getEntity(name, ENTITY_TYPE_STR, getThis());
		RETURN_ZVAL(&entity, 1, 1);
	}

	// Throw \Trogord\NetworkException
	catch (const NetworkException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(networkException), e.what(), 0);
		RETURN_NULL();
	}

	catch (const RequestException &e) {

		if (404 == e.getCode()) {

			// Throw \Trogdord\GameNotFound
			if (0 == strcmp(e.what(), "game not found")) {
				zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), e.getCode());
				RETURN_NULL();
			}

			// Throw \Trogdord\EntityNotFound
			else {
				zend_throw_exception(EXCEPTION_GLOBALS(entityNotFound), e.what(), e.getCode());
				RETURN_NULL();
			}
		}

		// Throw \Trogdord\RequestException
		else {
			zend_throw_exception(EXCEPTION_GLOBALS(requestException), e.what(), e.getCode());
			RETURN_NULL();
		}
	}
}

/*****************************************************************************/

// Returns an object that inherits from \Trogdord\Tangible representing the
// specified tangible in the game. Throws an instance of
// \Trogdord\NetworkException if the call fails due to network connectivity
// issues, an instance of \Trogdord\GameNotFound if the game has since been
// destroyed and no longer exists, and an instance of Trogdord\TangibleNotFound
// if the specified tangible does not exist.
ZEND_BEGIN_ARG_INFO(arginfoGetTangible, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Game, getTangible) {

	char *name;
	size_t nameLength;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name, &nameLength) == FAILURE) {
		RETURN_NULL();
	}

	zval rv;
	zval *gameId = GAME_TO_ID(getThis(), &rv);

	ASSERT_GAME_ID_IS_VALID(Z_TYPE_P(gameId));

	try {
		zval entity = getEntity(name, TANGIBLE_TYPE_STR, getThis());
		RETURN_ZVAL(&entity, 1, 1);
	}

	// Throw \Trogord\NetworkException
	catch (const NetworkException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(networkException), e.what(), 0);
		RETURN_NULL();
	}

	catch (const RequestException &e) {

		if (404 == e.getCode()) {

			// Throw \Trogdord\GameNotFound
			if (0 == strcmp(e.what(), "game not found")) {
				zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), e.getCode());
				RETURN_NULL();
			}

			// Throw \Trogdord\TangibleNotFound
			else {
				zend_throw_exception(EXCEPTION_GLOBALS(tangibleNotFound), e.what(), e.getCode());
				RETURN_NULL();
			}
		}

		// Throw \Trogdord\RequestException
		else {
			zend_throw_exception(EXCEPTION_GLOBALS(requestException), e.what(), e.getCode());
			RETURN_NULL();
		}
	}
}

/*****************************************************************************/

// Returns an object that inherits from \Trogdord\Place representing the
// specified place in the game. Throws an instance of
// \Trogdord\NetworkException if the call fails due to network connectivity
// issues, an instance of \Trogdord\GameNotFound if the game has since been
// destroyed and no longer exists, and an instance of Trogdord\PlaceNotFound if
// the specified place does not exist.
ZEND_BEGIN_ARG_INFO(arginfoGetPlace, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Game, getPlace) {

	char *name;
	size_t nameLength;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name, &nameLength) == FAILURE) {
		RETURN_NULL();
	}

	zval rv;
	zval *gameId = GAME_TO_ID(getThis(), &rv);

	ASSERT_GAME_ID_IS_VALID(Z_TYPE_P(gameId));

	try {
		zval entity = getEntity(name, PLACE_TYPE_STR, getThis());
		RETURN_ZVAL(&entity, 1, 1);
	}

	// Throw \Trogord\NetworkException
	catch (const NetworkException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(networkException), e.what(), 0);
		RETURN_NULL();
	}

	catch (const RequestException &e) {

		if (404 == e.getCode()) {

			// Throw \Trogdord\GameNotFound
			if (0 == strcmp(e.what(), "game not found")) {
				zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), e.getCode());
				RETURN_NULL();
			}

			// Throw \Trogdord\PlaceNotFound
			else {
				zend_throw_exception(EXCEPTION_GLOBALS(placeNotFound), e.what(), e.getCode());
				RETURN_NULL();
			}
		}

		// Throw \Trogdord\RequestException
		else {
			zend_throw_exception(EXCEPTION_GLOBALS(requestException), e.what(), e.getCode());
			RETURN_NULL();
		}
	}
}

/*****************************************************************************/

// Returns an object that inherits from \Trogdord\Thing representing the
// specified thing in the game. Throws an instance of
// \Trogdord\NetworkException if the call fails due to network connectivity
// issues, an instance of \Trogdord\GameNotFound if the game has since been
// destroyed and no longer exists, and an instance of Trogdord\ThingNotFound if
// the specified thing does not exist.
ZEND_BEGIN_ARG_INFO(arginfoGetThing, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Game, getThing) {

	char *name;
	size_t nameLength;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name, &nameLength) == FAILURE) {
		RETURN_NULL();
	}

	zval rv;
	zval *gameId = GAME_TO_ID(getThis(), &rv);

	ASSERT_GAME_ID_IS_VALID(Z_TYPE_P(gameId));

	try {
		zval entity = getEntity(name, THING_TYPE_STR, getThis());
		RETURN_ZVAL(&entity, 1, 1);
	}

	// Throw \Trogord\NetworkException
	catch (const NetworkException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(networkException), e.what(), 0);
		RETURN_NULL();
	}

	catch (const RequestException &e) {

		if (404 == e.getCode()) {

			// Throw \Trogdord\GameNotFound
			if (0 == strcmp(e.what(), "game not found")) {
				zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), e.getCode());
				RETURN_NULL();
			}

			// Throw \Trogdord\ThingNotFound
			else {
				zend_throw_exception(EXCEPTION_GLOBALS(thingNotFound), e.what(), e.getCode());
				RETURN_NULL();
			}
		}

		// Throw \Trogdord\RequestException
		else {
			zend_throw_exception(EXCEPTION_GLOBALS(requestException), e.what(), e.getCode());
			RETURN_NULL();
		}
	}
}

/*****************************************************************************/

// Returns an object that inherits from \Trogdord\Being representing the
// specified being in the game. Throws an instance of
// \Trogdord\NetworkException if the call fails due to network connectivity
// issues, an instance of \Trogdord\GameNotFound if the game has since been
// destroyed and no longer exists, and an instance of Trogdord\BeingNotFound if
// the specified being does not exist.
ZEND_BEGIN_ARG_INFO(arginfoGetBeing, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Game, getBeing) {

	char *name;
	size_t nameLength;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name, &nameLength) == FAILURE) {
		RETURN_NULL();
	}

	zval rv;
	zval *gameId = GAME_TO_ID(getThis(), &rv);

	ASSERT_GAME_ID_IS_VALID(Z_TYPE_P(gameId));

	try {
		zval entity = getEntity(name, BEING_TYPE_STR, getThis());
		RETURN_ZVAL(&entity, 1, 1);
	}

	// Throw \Trogord\NetworkException
	catch (const NetworkException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(networkException), e.what(), 0);
		RETURN_NULL();
	}

	catch (const RequestException &e) {

		if (404 == e.getCode()) {

			// Throw \Trogdord\GameNotFound
			if (0 == strcmp(e.what(), "game not found")) {
				zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), e.getCode());
				RETURN_NULL();
			}

			// Throw \Trogdord\BeingNotFound
			else {
				zend_throw_exception(EXCEPTION_GLOBALS(beingNotFound), e.what(), e.getCode());
				RETURN_NULL();
			}
		}

		// Throw \Trogdord\RequestException
		else {
			zend_throw_exception(EXCEPTION_GLOBALS(requestException), e.what(), e.getCode());
			RETURN_NULL();
		}
	}
}

/*****************************************************************************/

// Returns an instance of \Trogdord\Resource representing the specified resource
// in the game. Throws an instance of \Trogdord\NetworkException if the call
// fails due to network connectivity issues, an instance of
// \Trogdord\GameNotFound if the game has since been destroyed and no longer
// exists, and an instance of Trogdord\ResourceNotFound if the specified
// resource does not exist.
ZEND_BEGIN_ARG_INFO(arginfoGetResource, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Game, getResource) {

	char *name;
	size_t nameLength;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name, &nameLength) == FAILURE) {
		RETURN_NULL();
	}

	zval rv;
	zval *gameId = GAME_TO_ID(getThis(), &rv);

	ASSERT_GAME_ID_IS_VALID(Z_TYPE_P(gameId));

	try {
		zval entity = getEntity(name, RESOURCE_TYPE_STR, getThis());
		RETURN_ZVAL(&entity, 1, 1);
	}

	// Throw \Trogord\NetworkException
	catch (const NetworkException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(networkException), e.what(), 0);
		RETURN_NULL();
	}

	catch (const RequestException &e) {

		if (404 == e.getCode()) {

			// Throw \Trogdord\GameNotFound
			if (0 == strcmp(e.what(), "game not found")) {
				zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), e.getCode());
				RETURN_NULL();
			}

			// Throw \Trogdord\ResourceNotFound
			else {
				zend_throw_exception(EXCEPTION_GLOBALS(resourceNotFound), e.what(), e.getCode());
				RETURN_NULL();
			}
		}

		// Throw \Trogdord\RequestException
		else {
			zend_throw_exception(EXCEPTION_GLOBALS(requestException), e.what(), e.getCode());
			RETURN_NULL();
		}
	}
}

/*****************************************************************************/

// Returns an instance of \Trogdord\Room representing the specified room in the
// game. Throws an instance of \Trogdord\NetworkException if the call fails due
// to network connectivity issues, an instance of \Trogdord\GameNotFound if the
// game has since been destroyed and no longer exists, and an instance of
// Trogdord\RoomNotFound if the specified room does not exist.
ZEND_BEGIN_ARG_INFO(arginfoGetRoom, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Game, getRoom) {

	char *name;
	size_t nameLength;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name, &nameLength) == FAILURE) {
		RETURN_NULL();
	}

	zval rv;
	zval *gameId = GAME_TO_ID(getThis(), &rv);

	ASSERT_GAME_ID_IS_VALID(Z_TYPE_P(gameId));

	try {
		zval entity = getEntity(name, ROOM_TYPE_STR, getThis());
		RETURN_ZVAL(&entity, 1, 1);
	}

	// Throw \Trogord\NetworkException
	catch (const NetworkException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(networkException), e.what(), 0);
		RETURN_NULL();
	}

	catch (const RequestException &e) {

		if (404 == e.getCode()) {

			// Throw \Trogdord\GameNotFound
			if (0 == strcmp(e.what(), "game not found")) {
				zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), e.getCode());
				RETURN_NULL();
			}

			// Throw \Trogdord\RoomNotFound
			else {
				zend_throw_exception(EXCEPTION_GLOBALS(roomNotFound), e.what(), e.getCode());
				RETURN_NULL();
			}
		}

		// Throw \Trogdord\RequestException
		else {
			zend_throw_exception(EXCEPTION_GLOBALS(requestException), e.what(), e.getCode());
			RETURN_NULL();
		}
	}
}

/*****************************************************************************/

// Returns an instance of \Trogdord\Object representing the specified object in
// the game. Throws an instance of \Trogdord\NetworkException if the call fails
// due to network connectivity issues, an instance of \Trogdord\GameNotFound if
// the game has since been destroyed and no longer exists, and an instance of
// Trogdord\ObjectNotFound if the specified object does not exist.
ZEND_BEGIN_ARG_INFO(arginfoGetObject, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Game, getObject) {

	char *name;
	size_t nameLength;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name, &nameLength) == FAILURE) {
		RETURN_NULL();
	}

	zval rv;
	zval *gameId = GAME_TO_ID(getThis(), &rv);

	ASSERT_GAME_ID_IS_VALID(Z_TYPE_P(gameId));

	try {
		zval entity = getEntity(name, OBJECT_TYPE_STR, getThis());
		RETURN_ZVAL(&entity, 1, 1);
	}

	// Throw \Trogord\NetworkException
	catch (const NetworkException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(networkException), e.what(), 0);
		RETURN_NULL();
	}

	catch (const RequestException &e) {

		if (404 == e.getCode()) {

			// Throw \Trogdord\GameNotFound
			if (0 == strcmp(e.what(), "game not found")) {
				zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), e.getCode());
				RETURN_NULL();
			}

			// Throw \Trogdord\ObjectNotFound
			else {
				zend_throw_exception(EXCEPTION_GLOBALS(objectNotFound), e.what(), e.getCode());
				RETURN_NULL();
			}
		}

		// Throw \Trogdord\RequestException
		else {
			zend_throw_exception(EXCEPTION_GLOBALS(requestException), e.what(), e.getCode());
			RETURN_NULL();
		}
	}
}

/*****************************************************************************/

// Returns an instance of \Trogdord\Creature representing the specified creature
// in the game. Throws an instance of \Trogdord\NetworkException if the call
// fails due to network connectivity issues, an instance of
// \Trogdord\GameNotFound if the game has since been destroyed and no longer
// exists, and an instance of Trogdord\CreatureNotFound if the specified
// creature does not exist.
ZEND_BEGIN_ARG_INFO(arginfoGetCreature, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Game, getCreature) {

	char *name;
	size_t nameLength;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name, &nameLength) == FAILURE) {
		RETURN_NULL();
	}

	zval rv;
	zval *gameId = GAME_TO_ID(getThis(), &rv);

	ASSERT_GAME_ID_IS_VALID(Z_TYPE_P(gameId));

	try {
		zval entity = getEntity(name, CREATURE_TYPE_STR, getThis());
		RETURN_ZVAL(&entity, 1, 1);
	}

	// Throw \Trogord\NetworkException
	catch (const NetworkException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(networkException), e.what(), 0);
		RETURN_NULL();
	}

	catch (const RequestException &e) {

		if (404 == e.getCode()) {

			// Throw \Trogdord\GameNotFound
			if (0 == strcmp(e.what(), "game not found")) {
				zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), e.getCode());
				RETURN_NULL();
			}

			// Throw \Trogdord\CreatureNotFound
			else {
				zend_throw_exception(EXCEPTION_GLOBALS(creatureNotFound), e.what(), e.getCode());
				RETURN_NULL();
			}
		}

		// Throw \Trogdord\RequestException
		else {
			zend_throw_exception(EXCEPTION_GLOBALS(requestException), e.what(), e.getCode());
			RETURN_NULL();
		}
	}
}

/*****************************************************************************/

// Returns an instance of \Trogdord\Player representing the specified player in
// the game. Throws an instance of \Trogdord\NetworkException if the call fails
// due to network connectivity issues, an instance of \Trogdord\GameNotFound if
// the game has since been destroyed and no longer exists, and an instance of
// Trogdord\PlayerNotFound if the specified player does not exist.
ZEND_BEGIN_ARG_INFO(arginfoGetPlayer, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Game, getPlayer) {

	char *name;
	size_t nameLength;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name, &nameLength) == FAILURE) {
		RETURN_NULL();
	}

	zval rv;
	zval *gameId = GAME_TO_ID(getThis(), &rv);

	ASSERT_GAME_ID_IS_VALID(Z_TYPE_P(gameId));

	try {
		zval entity = getEntity(name, PLAYER_TYPE_STR, getThis());
		RETURN_ZVAL(&entity, 1, 1);
	}

	// Throw \Trogord\NetworkException
	catch (const NetworkException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(networkException), e.what(), 0);
		RETURN_NULL();
	}

	catch (const RequestException &e) {

		if (404 == e.getCode()) {

			// Throw \Trogdord\GameNotFound
			if (0 == strcmp(e.what(), "game not found")) {
				zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), e.getCode());
				RETURN_NULL();
			}

			// Throw \Trogdord\PlayerNotFound
			else {
				zend_throw_exception(EXCEPTION_GLOBALS(playerNotFound), e.what(), e.getCode());
				RETURN_NULL();
			}
		}

		// Throw \Trogdord\RequestException
		else {
			zend_throw_exception(EXCEPTION_GLOBALS(requestException), e.what(), e.getCode());
			RETURN_NULL();
		}
	}
}

/*****************************************************************************/

// Creates a new player in the game and returns and instance of \Trogdord\Player
// that represents it. Throws an instance of \Trogdord\NetworkException if the
// call fails due to network connectivity issues, an instance of
// \Trogdord\GameNotFound if the game has since been destroyed and no longer
// exists, and an instance of Trogdord\RequestException with the appropriate
// error code and message set if the player by the given name already exists.
ZEND_BEGIN_ARG_INFO(arginfoCreatePlayer, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Game, createPlayer) {

	char *name;
	size_t nameLength;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name, &nameLength) == FAILURE) {
		RETURN_NULL();
	}

	zval rv; // ???

	zval *trogdord = GAME_TO_TROGDORD(getThis(), &rv);
	zval *id = GAME_TO_ID(getThis(), &rv);

	ASSERT_GAME_ID_IS_VALID(Z_TYPE_P(id));

	try {

		std::string request = NEW_PLAYER_REQUEST;
		strReplace(request, "%gid", std::to_string(Z_LVAL_P(id)));
		strReplace(request, "%pname", name);

		trogdordObject *objWrapper = ZOBJ_TO_TROGDORD(Z_OBJ_P(trogdord));

		Document response = Request::execute(
			objWrapper->data.hostname,
			objWrapper->data.port,
			request,
			trogdord
		);

		Document player;

		Value JSONName;
		Value JSONType;

		player.SetObject();

		JSONName.SetString(name, nameLength, player.GetAllocator());
		JSONType.SetString(PLAYER_TYPE_STR, strlen(PLAYER_TYPE_STR), player.GetAllocator());

		player.AddMember("name", JSONName, player.GetAllocator());
		player.AddMember("type", JSONType, player.GetAllocator());

		if (!createEntityObj(return_value, player, getThis())) {
			php_error_docref(NULL, E_ERROR, "failed to instantiate Trogdord\\Player");
		}

		return;
	}

	// Throw \Trogord\NetworkException
	catch (const NetworkException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(networkException), e.what(), 0);
		RETURN_NULL();
	}

	catch (const RequestException &e) {

		// Throw \Trogdord\GameNotFound
		if (404 == e.getCode()) {
			zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), e.getCode());
			RETURN_NULL();
		}

		// Throw \Trogdord\RequestException
		else {
			zend_throw_exception(EXCEPTION_GLOBALS(requestException), e.what(), e.getCode());
			RETURN_NULL();
		}
	}
}

/*****************************************************************************/

// PHP Game class methods
static const zend_function_entry classMethods[] =  {
	PHP_ME(Game, __construct, arginfoCtor, ZEND_ACC_PRIVATE | ZEND_ACC_CTOR)
	PHP_ME(Game, __get, arginfoMagicGet, ZEND_ACC_PUBLIC)
	PHP_ME(Game, start, arginfoStart, ZEND_ACC_PUBLIC)
	PHP_ME(Game, stop, arginfoStop, ZEND_ACC_PUBLIC)
	PHP_ME(Game, getTime, arginfoGetTime, ZEND_ACC_PUBLIC)
	PHP_ME(Game, isRunning, arginfoIsRunning, ZEND_ACC_PUBLIC)
	PHP_ME(Game, statistics, arginfoStatistics, ZEND_ACC_PUBLIC)
	PHP_ME(Game, destroy, arginfoDestroy, ZEND_ACC_PUBLIC)
	PHP_ME(Game, getMeta, arginfoGetMeta, ZEND_ACC_PUBLIC)
	PHP_ME(Game, setMeta, arginfoSetMeta, ZEND_ACC_PUBLIC)
	PHP_ME(Game, entities, arginfoListEntities, ZEND_ACC_PUBLIC)
	PHP_ME(Game, tangibles, arginfoListTangibles, ZEND_ACC_PUBLIC)
	PHP_ME(Game, places, arginfoListPlaces, ZEND_ACC_PUBLIC)
	PHP_ME(Game, things, arginfoListThings, ZEND_ACC_PUBLIC)
	PHP_ME(Game, beings, arginfoListBeings, ZEND_ACC_PUBLIC)
	PHP_ME(Game, resources, arginfoListResources, ZEND_ACC_PUBLIC)
	PHP_ME(Game, rooms, arginfoListRooms, ZEND_ACC_PUBLIC)
	PHP_ME(Game, objects, arginfoListObjects, ZEND_ACC_PUBLIC)
	PHP_ME(Game, creatures, arginfoListCreatures, ZEND_ACC_PUBLIC)
	PHP_ME(Game, players, arginfoListPlayers, ZEND_ACC_PUBLIC)
	PHP_ME(Game, getEntity, arginfoGetEntity, ZEND_ACC_PUBLIC)
	PHP_ME(Game, getTangible, arginfoGetTangible, ZEND_ACC_PUBLIC)
	PHP_ME(Game, getPlace, arginfoGetPlace, ZEND_ACC_PUBLIC)
	PHP_ME(Game, getThing, arginfoGetThing, ZEND_ACC_PUBLIC)
	PHP_ME(Game, getBeing, arginfoGetBeing, ZEND_ACC_PUBLIC)
	PHP_ME(Game, getResource, arginfoGetResource, ZEND_ACC_PUBLIC)
	PHP_ME(Game, getRoom, arginfoGetRoom, ZEND_ACC_PUBLIC)
	PHP_ME(Game, getObject, arginfoGetObject, ZEND_ACC_PUBLIC)
	PHP_ME(Game, getCreature, arginfoGetCreature, ZEND_ACC_PUBLIC)
	PHP_ME(Game, getPlayer, arginfoGetPlayer, ZEND_ACC_PUBLIC)
	PHP_ME(Game, createPlayer, arginfoCreatePlayer, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/*****************************************************************************/

// Note: cppcheck will suggest you pass type by reference, but doing so will
// break the build, so please ignore its advice.
static zval getEntityList(size_t gameId, std::string type, zval *trogdord) {

	std::string request = ENTITY_LIST_REQUEST;

	strReplace(request, "%gid", std::to_string(gameId));
	strReplace(request, "%etype", type);

	trogdordObject *objWrapper = ZOBJ_TO_TROGDORD(Z_OBJ_P(trogdord));

	Document response = Request::execute(
		objWrapper->data.hostname,
		objWrapper->data.port,
		request,
		trogdord
	);

	return JSON::JSONToZval(response["entities"]);
}

/*****************************************************************************/

// Note: cppcheck will suggest you pass name and type by reference, but doing so
// will break the build, so please ignore its advice.
static zval getEntity(std::string name, std::string type, zval *game) {

	std::string request = ENTITY_GET_REQUEST;
	zval rv; // ???

	zval *gameId = GAME_TO_ID(game, &rv);
	zval *trogdord = GAME_TO_TROGDORD(game, &rv);

	strReplace(request, "%gid", std::to_string(Z_LVAL_P(gameId)));
	strReplace(request, "%etype", type);
	strReplace(request, "%ename", name);

	trogdordObject *objWrapper = ZOBJ_TO_TROGDORD(Z_OBJ_P(trogdord));

	Document response = Request::execute(
		objWrapper->data.hostname,
		objWrapper->data.port,
		request,
		trogdord
	);

	zval entity;

	if (!createEntityObj(&entity, response["entity"], game)) {
		php_error_docref(NULL, E_ERROR, "failed to instantiate entity");
	}

	return entity;
}

/*****************************************************************************/

bool createGameObj(
	zval *gameObj,
	std::string name,
	std::string definition,
	size_t id,
	zval *trogdordObj
) {

	if (SUCCESS != object_init_ex(gameObj, GAME_GLOBALS(classEntry))) {
		return false;
	}

	zend_update_property_string(
		GAME_GLOBALS(classEntry),
		#if ZEND_MODULE_API_NO >= 20200930 // PHP 8.0+
			Z_OBJ_P(gameObj),
		#else
			gameObj,
		#endif
		GAME_NAME_PROPERTY,
		strlen(GAME_NAME_PROPERTY),
		name.c_str()
	);

	zend_update_property_string(
		GAME_GLOBALS(classEntry),
		#if ZEND_MODULE_API_NO >= 20200930 // PHP 8.0+
			Z_OBJ_P(gameObj),
		#else
			gameObj,
		#endif
		GAME_DEFINITION_PROPERTY,
		strlen(GAME_DEFINITION_PROPERTY),
		definition.c_str()
	);

	zend_update_property_long(
		GAME_GLOBALS(classEntry),
		#if ZEND_MODULE_API_NO >= 20200930 // PHP 8.0+
			Z_OBJ_P(gameObj),
		#else
			gameObj,
		#endif
		GAME_ID_PROPERTY,
		strlen(GAME_ID_PROPERTY),
		id
	);

	zend_update_property(
		GAME_GLOBALS(classEntry),
		#if ZEND_MODULE_API_NO >= 20200930 // PHP 8.0+
			Z_OBJ_P(gameObj),
		#else
			gameObj,
		#endif
		TROGDORD_PROPERTY,
		strlen(TROGDORD_PROPERTY),
		trogdordObj
	);

	return true;
}

/*****************************************************************************/

void defineGameClass() {

	zend_class_entry gameClass;

	INIT_CLASS_ENTRY(gameClass, "Trogdord\\Game", classMethods);
	GAME_GLOBALS(classEntry) = zend_register_internal_class(&gameClass);

	zend_declare_property_null(
		GAME_GLOBALS(classEntry),
		GAME_NAME_PROPERTY,
		strlen(GAME_NAME_PROPERTY),
		ZEND_ACC_PRIVATE
	);

	zend_declare_property_null(
		GAME_GLOBALS(classEntry),
		GAME_DEFINITION_PROPERTY,
		strlen(GAME_DEFINITION_PROPERTY),
		ZEND_ACC_PRIVATE
	);

	zend_declare_property_null(
		GAME_GLOBALS(classEntry),
		GAME_ID_PROPERTY,
		strlen(GAME_ID_PROPERTY),
		ZEND_ACC_PRIVATE
	);

	zend_declare_property_null(
		GAME_GLOBALS(classEntry),
		TROGDORD_PROPERTY,
		strlen(TROGDORD_PROPERTY),
		ZEND_ACC_PRIVATE
	);

	// Make sure users can't extend the class
	GAME_GLOBALS(classEntry)->ce_flags |= ZEND_ACC_FINAL;
}
