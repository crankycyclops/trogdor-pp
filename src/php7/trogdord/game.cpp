#include "trogdord.h"
#include "json.h"
#include "request.h"
#include "utility.h"

#include "phpexception.h"
#include "exception/requestexception.h"

#include "game.h"
#include "entities/entity.h"

ZEND_DECLARE_MODULE_GLOBALS(game);
ZEND_EXTERN_MODULE_GLOBALS(trogdord);

// Exception message when methods are called on a game that's already been destroyed
static const char *GAME_ALREADY_DESTROYED = "Game has already been destroyed";

// The private property that stores the game's id
const char *GAME_ID_PROPERTY_NAME = "id";

// The private property through which an instance of \Trogdord\Game can access
// the connection that spawned it
const char *TROGDORD_PROPERTY_NAME = "trogdord";

// This request starts the game
static const char *GAME_START_REQUEST = "{\"method\":\"set\",\"scope\":\"game\",\"action\":\"start\",\"args\":{\"id\": %gid}}";

// This request stops the game
static const char *GAME_STOP_REQUEST = "{\"method\":\"set\",\"scope\":\"game\",\"action\":\"stop\",\"args\":{\"id\": %gid}}";

// This request destroys the game
static const char *GAME_DESTROY_REQUEST = "{\"method\":\"delete\",\"scope\":\"game\",\"args\":{\"id\": %gid}}";

// This request returns a list of entities of the specified type
static const char *ENTITY_LIST_REQUEST = "{\"method\":\"get\",\"scope\":\"%etype\",\"action\":\"list\",\"args\":{\"game_id\":%gid}}";

// This request returns the specified entity (if it exists)
static const char *ENTITY_GET_REQUEST = "{\"method\":\"get\",\"scope\":\"%etype\",\"args\":{\"game_id\":%gid,\"name\":\"%ename\"}}";

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
	int keyLength;

	zval *propVal, rv;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &keyLength) == FAILURE) {
		RETURN_NULL()
	}

	// If I return whatever zend_read_property sets rv to, I get an UNKNOWN
	// zval type when I var_dump the result, but when I return
	// zend_read_property's return value instead, it works like it's supposed
	// to. WTF is the rv argument for? Dunno, because PHP's documentation
	// regarding internals is no good. Oh well.
	propVal = zend_read_property(
		GAME_GLOBALS(classEntry),
		getThis(),
		key,
		keyLength,
		1,
		&rv TSRMLS_CC
	);

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

	zval rv;

	zval *trogdord = zend_read_property(
		GAME_GLOBALS(classEntry),
		getThis(),
		TROGDORD_PROPERTY_NAME,
		strlen(TROGDORD_PROPERTY_NAME),
		1,
		&rv TSRMLS_CC
	);

	zval *id = zend_read_property(
		GAME_GLOBALS(classEntry),
		getThis(),
		GAME_ID_PROPERTY_NAME,
		strlen(GAME_ID_PROPERTY_NAME),
		1,
		&rv TSRMLS_CC
	);

	if (IS_NULL == Z_TYPE_P(id)) {
		zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), GAME_ALREADY_DESTROYED, 0);
		RETURN_NULL();
	}

	try {

		std::string request = GAME_START_REQUEST;
		strReplace(request, "%gid", std::to_string(Z_LVAL_P(id)));

		trogdordObject *objWrapper = ZOBJ_TO_TROGDORD(Z_OBJ_P(trogdord));

		JSONObject response = Request::execute(
			objWrapper->data.hostname,
			objWrapper->data.port,
			request
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
			zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), 0);
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

	zval rv;

	zval *trogdord = zend_read_property(
		GAME_GLOBALS(classEntry),
		getThis(),
		TROGDORD_PROPERTY_NAME,
		strlen(TROGDORD_PROPERTY_NAME),
		1,
		&rv TSRMLS_CC
	);

	zval *id = zend_read_property(
		GAME_GLOBALS(classEntry),
		getThis(),
		GAME_ID_PROPERTY_NAME,
		strlen(GAME_ID_PROPERTY_NAME),
		1,
		&rv TSRMLS_CC
	);

	if (IS_NULL == Z_TYPE_P(id)) {
		zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), GAME_ALREADY_DESTROYED, 0);
		RETURN_NULL();
	}

	try {

		std::string request = GAME_STOP_REQUEST;
		strReplace(request, "%gid", std::to_string(Z_LVAL_P(id)));

		trogdordObject *objWrapper = ZOBJ_TO_TROGDORD(Z_OBJ_P(trogdord));

		JSONObject response = Request::execute(
			objWrapper->data.hostname,
			objWrapper->data.port,
			request
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
			zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), 0);
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
ZEND_BEGIN_ARG_INFO(arginfoDestroy, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Game, destroy) {

	zval rv;

	zval *trogdord = zend_read_property(
		GAME_GLOBALS(classEntry),
		getThis(),
		TROGDORD_PROPERTY_NAME,
		strlen(TROGDORD_PROPERTY_NAME),
		1,
		&rv TSRMLS_CC
	);

	zval *id = zend_read_property(
		GAME_GLOBALS(classEntry),
		getThis(),
		GAME_ID_PROPERTY_NAME,
		strlen(GAME_ID_PROPERTY_NAME),
		1,
		&rv TSRMLS_CC
	);

	if (IS_NULL == Z_TYPE_P(id)) {
		zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), GAME_ALREADY_DESTROYED, 0);
		RETURN_NULL();
	}

	try {

		std::string request = GAME_DESTROY_REQUEST;
		strReplace(request, "%gid", std::to_string(Z_LVAL_P(id)));

		trogdordObject *objWrapper = ZOBJ_TO_TROGDORD(Z_OBJ_P(trogdord));

		JSONObject response = Request::execute(
			objWrapper->data.hostname,
			objWrapper->data.port,
			request
		);

		// Set the game ID to null so the object can't be used anymore
		zend_update_property_null(
			GAME_GLOBALS(classEntry),
			getThis(),
			GAME_ID_PROPERTY_NAME,
			strlen(GAME_ID_PROPERTY_NAME)
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
			zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), 0);
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
		zval list = getEntityList(Z_LVAL_P(id), "entity", trogdord);
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
			zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), 0);
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
		zval list = getEntityList(Z_LVAL_P(id), "place", trogdord);
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
			zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), 0);
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
		zval list = getEntityList(Z_LVAL_P(id), "thing", trogdord);
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
			zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), 0);
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
		zval list = getEntityList(Z_LVAL_P(id), "being", trogdord);
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
			zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), 0);
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
		zval list = getEntityList(Z_LVAL_P(id), "room", trogdord);
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
			zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), 0);
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
		zval list = getEntityList(Z_LVAL_P(id), "object", trogdord);
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
			zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), 0);
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
		zval list = getEntityList(Z_LVAL_P(id), "creature", trogdord);
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
			zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), 0);
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
		zval list = getEntityList(Z_LVAL_P(id), "player", trogdord);
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
			zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), 0);
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
	int nameLength;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &nameLength) == FAILURE) {
		RETURN_NULL();
	}

	zval rv;
	zval *gameId = GAME_TO_ID(getThis(), &rv);

	ASSERT_GAME_ID_IS_VALID(Z_TYPE_P(gameId));

	try {
		zval entity = getEntity(name, "entity", getThis());
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
				zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), 0);
				RETURN_NULL();
			}

			// Throw \Trogdord\EntityNotFound
			else {
				zend_throw_exception(EXCEPTION_GLOBALS(entityNotFound), e.what(), 0);
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
	int nameLength;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &nameLength) == FAILURE) {
		RETURN_NULL();
	}

	zval rv;
	zval *gameId = GAME_TO_ID(getThis(), &rv);

	ASSERT_GAME_ID_IS_VALID(Z_TYPE_P(gameId));

	try {
		zval entity = getEntity(name, "place", getThis());
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
				zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), 0);
				RETURN_NULL();
			}

			// Throw \Trogdord\PlaceNotFound
			else {
				zend_throw_exception(EXCEPTION_GLOBALS(placeNotFound), e.what(), 0);
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
	int nameLength;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &nameLength) == FAILURE) {
		RETURN_NULL();
	}

	zval rv;
	zval *gameId = GAME_TO_ID(getThis(), &rv);

	ASSERT_GAME_ID_IS_VALID(Z_TYPE_P(gameId));

	try {
		zval entity = getEntity(name, "thing", getThis());
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
				zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), 0);
				RETURN_NULL();
			}

			// Throw \Trogdord\ThingNotFound
			else {
				zend_throw_exception(EXCEPTION_GLOBALS(thingNotFound), e.what(), 0);
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
	int nameLength;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &nameLength) == FAILURE) {
		RETURN_NULL();
	}

	zval rv;
	zval *gameId = GAME_TO_ID(getThis(), &rv);

	ASSERT_GAME_ID_IS_VALID(Z_TYPE_P(gameId));

	try {
		zval entity = getEntity(name, "being", getThis());
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
				zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), 0);
				RETURN_NULL();
			}

			// Throw \Trogdord\BeingNotFound
			else {
				zend_throw_exception(EXCEPTION_GLOBALS(beingNotFound), e.what(), 0);
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
	int nameLength;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &nameLength) == FAILURE) {
		RETURN_NULL();
	}

	zval rv;
	zval *gameId = GAME_TO_ID(getThis(), &rv);

	ASSERT_GAME_ID_IS_VALID(Z_TYPE_P(gameId));

	try {
		zval entity = getEntity(name, "room", getThis());
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
				zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), 0);
				RETURN_NULL();
			}

			// Throw \Trogdord\RoomNotFound
			else {
				zend_throw_exception(EXCEPTION_GLOBALS(roomNotFound), e.what(), 0);
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
	int nameLength;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &nameLength) == FAILURE) {
		RETURN_NULL();
	}

	zval rv;
	zval *gameId = GAME_TO_ID(getThis(), &rv);

	ASSERT_GAME_ID_IS_VALID(Z_TYPE_P(gameId));

	try {
		zval entity = getEntity(name, "object", getThis());
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
				zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), 0);
				RETURN_NULL();
			}

			// Throw \Trogdord\ObjectNotFound
			else {
				zend_throw_exception(EXCEPTION_GLOBALS(objectNotFound), e.what(), 0);
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
	int nameLength;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &nameLength) == FAILURE) {
		RETURN_NULL();
	}

	zval rv;
	zval *gameId = GAME_TO_ID(getThis(), &rv);

	ASSERT_GAME_ID_IS_VALID(Z_TYPE_P(gameId));

	try {
		zval entity = getEntity(name, "creature", getThis());
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
				zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), 0);
				RETURN_NULL();
			}

			// Throw \Trogdord\CreatureNotFound
			else {
				zend_throw_exception(EXCEPTION_GLOBALS(creatureNotFound), e.what(), 0);
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
	int nameLength;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &nameLength) == FAILURE) {
		RETURN_NULL();
	}

	zval rv;
	zval *gameId = GAME_TO_ID(getThis(), &rv);

	ASSERT_GAME_ID_IS_VALID(Z_TYPE_P(gameId));

	try {
		zval entity = getEntity(name, "player", getThis());
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
				zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), 0);
				RETURN_NULL();
			}

			// Throw \Trogdord\PlayerNotFound
			else {
				zend_throw_exception(EXCEPTION_GLOBALS(playerNotFound), e.what(), 0);
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
	int nameLength;

	zval *propVal, rv;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &nameLength) == FAILURE) {
		RETURN_NULL()
	}

	// TODO
}

/*****************************************************************************/

// Destroys a player in the game. Throws an instance of
// \Trogdord\NetworkException if the call fails due to network connectivity
// issues, an instance of \Trogdord\GameNotFound if the game has since been
// destroyed and no longer exists, and an instance of Trogdord\PlayerNotFound if
// the specified player doesn't exist.
ZEND_BEGIN_ARG_INFO(arginfoDestroyPlayer, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Game, destroyPlayer) {

	char *name;
	int nameLength;

	zval *propVal, rv;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &nameLength) == FAILURE) {
		RETURN_NULL()
	}

	// TODO
}

/*****************************************************************************/

// PHP Game class methods
static const zend_function_entry classMethods[] =  {
	PHP_ME(Game, __construct, arginfoCtor, ZEND_ACC_PRIVATE | ZEND_ACC_CTOR)
	PHP_ME(Game, __get, arginfoMagicGet, ZEND_ACC_PUBLIC)
	PHP_ME(Game, start, arginfoStart, ZEND_ACC_PUBLIC)
	PHP_ME(Game, stop, arginfoStop, ZEND_ACC_PUBLIC)
	PHP_ME(Game, destroy, arginfoDestroy, ZEND_ACC_PUBLIC)
	PHP_ME(Game, entities, arginfoListEntities, ZEND_ACC_PUBLIC)
	PHP_ME(Game, places, arginfoListPlaces, ZEND_ACC_PUBLIC)
	PHP_ME(Game, things, arginfoListThings, ZEND_ACC_PUBLIC)
	PHP_ME(Game, beings, arginfoListBeings, ZEND_ACC_PUBLIC)
	PHP_ME(Game, rooms, arginfoListRooms, ZEND_ACC_PUBLIC)
	PHP_ME(Game, objects, arginfoListObjects, ZEND_ACC_PUBLIC)
	PHP_ME(Game, creatures, arginfoListCreatures, ZEND_ACC_PUBLIC)
	PHP_ME(Game, players, arginfoListPlayers, ZEND_ACC_PUBLIC)
	PHP_ME(Game, getEntity, arginfoGetEntity, ZEND_ACC_PUBLIC)
	PHP_ME(Game, getPlace, arginfoGetPlace, ZEND_ACC_PUBLIC)
	PHP_ME(Game, getThing, arginfoGetThing, ZEND_ACC_PUBLIC)
	PHP_ME(Game, getBeing, arginfoGetBeing, ZEND_ACC_PUBLIC)
	PHP_ME(Game, getRoom, arginfoGetRoom, ZEND_ACC_PUBLIC)
	PHP_ME(Game, getObject, arginfoGetObject, ZEND_ACC_PUBLIC)
	PHP_ME(Game, getCreature, arginfoGetCreature, ZEND_ACC_PUBLIC)
	PHP_ME(Game, getPlayer, arginfoGetPlayer, ZEND_ACC_PUBLIC)
	PHP_ME(Game, createPlayer, arginfoCreatePlayer, ZEND_ACC_PUBLIC)
	PHP_ME(Game, destroyPlayer, arginfoDestroyPlayer, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/*****************************************************************************/

static zval getEntityList(size_t gameId, std::string type, zval *trogdord) {

	std::string request = ENTITY_LIST_REQUEST;

	strReplace(request, "%gid", std::to_string(gameId));
	strReplace(request, "%etype", type);

	trogdordObject *objWrapper = ZOBJ_TO_TROGDORD(Z_OBJ_P(trogdord));

	JSONObject response = Request::execute(
		objWrapper->data.hostname,
		objWrapper->data.port,
		request
	);

	return JSON::JSONToZval(response.get_child("entities"));
}

/*****************************************************************************/

static zval getEntity(std::string name, std::string type, zval *game) {

	std::string request = ENTITY_GET_REQUEST;
	zval rv; // ???

	zval *gameId = zend_read_property(
		GAME_GLOBALS(classEntry),
		game,
		GAME_ID_PROPERTY_NAME,
		strlen(GAME_ID_PROPERTY_NAME),
		1,
		&rv TSRMLS_CC
	);

	zval *trogdord = zend_read_property(
		GAME_GLOBALS(classEntry),
		game,
		TROGDORD_PROPERTY_NAME,
		strlen(TROGDORD_PROPERTY_NAME),
		1,
		&rv TSRMLS_CC
	);

	strReplace(request, "%gid", std::to_string(Z_LVAL_P(gameId)));
	strReplace(request, "%etype", type);
	strReplace(request, "%ename", name);

	trogdordObject *objWrapper = ZOBJ_TO_TROGDORD(Z_OBJ_P(trogdord));

	JSONObject response = Request::execute(
		objWrapper->data.hostname,
		objWrapper->data.port,
		request
	);

	zval entity;

	if (!createEntityObj(&entity, response.get_child("entity"), game)) {
		php_error_docref(NULL, E_ERROR, "failed to instantiate entity");
	}

	return entity;
}

/*****************************************************************************/

bool createGameObj(zval *gameObj, size_t id, zval *trogdordObj) {

	if (SUCCESS != object_init_ex(gameObj, GAME_GLOBALS(classEntry))) {
		return false;
	}

	zend_update_property_long(
		GAME_GLOBALS(classEntry),
		gameObj,
		GAME_ID_PROPERTY_NAME,
		strlen(GAME_ID_PROPERTY_NAME),
		id
	);

	zend_update_property(
		GAME_GLOBALS(classEntry),
		gameObj,
		TROGDORD_PROPERTY_NAME,
		strlen(TROGDORD_PROPERTY_NAME),
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
		GAME_ID_PROPERTY_NAME,
		strlen(GAME_ID_PROPERTY_NAME),
		ZEND_ACC_PRIVATE
		TSRMLS_CC
	);

	zend_declare_property_null(
		GAME_GLOBALS(classEntry),
		TROGDORD_PROPERTY_NAME,
		strlen(TROGDORD_PROPERTY_NAME),
		ZEND_ACC_PRIVATE
		TSRMLS_CC
	);

	// Make sure users can't extend the class
	GAME_GLOBALS(classEntry)->ce_flags |= ZEND_ACC_FINAL;
}
