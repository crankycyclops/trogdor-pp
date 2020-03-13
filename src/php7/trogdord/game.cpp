#include "trogdord.h"
#include "json.h"
#include "request.h"
#include "utility.h"

#include "phpexception.h"
#include "exception/requestexception.h"

#include "game.h"

ZEND_DECLARE_MODULE_GLOBALS(game);
ZEND_EXTERN_MODULE_GLOBALS(trogdord);

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

// Exception message when methods are called on a game that's already been destroyed
static const char *GAME_ALREADY_DESTROYED = "Game has already been destroyed";

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
	}

	catch (const RequestException &e) {

		// Throw \Trogdord\GameNotFound
		if (404 == e.getCode()) {
			zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), 0);
		}

		// Throw \Trogdord\RequestException
		else {
			zend_throw_exception(EXCEPTION_GLOBALS(requestException), e.what(), e.getCode());
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
	}

	catch (const RequestException &e) {

		// Throw \Trogdord\GameNotFound
		if (404 == e.getCode()) {
			zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), 0);
		}

		// Throw \Trogdord\RequestException
		else {
			zend_throw_exception(EXCEPTION_GLOBALS(requestException), e.what(), e.getCode());
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
	}

	catch (const RequestException &e) {

		// Throw \Trogdord\GameNotFound
		if (404 == e.getCode()) {
			zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), 0);
		}

		// Throw \Trogdord\RequestException
		else {
			zend_throw_exception(EXCEPTION_GLOBALS(requestException), e.what(), e.getCode());
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

PHP_METHOD(Game, listEntities) {

}

/*****************************************************************************/

// Returns a list of all places in the game. Throws an instance of
// \Trogdord\NetworkException if the call fails due to network connectivity
// issues and an instance of \Trogdord\GameNotFound if the game has since been
// destroyed and no longer exists.
ZEND_BEGIN_ARG_INFO(arginfoListPlaces, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Game, listPlaces) {

}

/*****************************************************************************/

// Returns a list of all things in the game. Throws an instance of
// \Trogdord\NetworkException if the call fails due to network connectivity
// issues and an instance of \Trogdord\GameNotFound if the game has since been
// destroyed and no longer exists.
ZEND_BEGIN_ARG_INFO(arginfoListThings, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Game, listThings) {

}

/*****************************************************************************/

// Returns a list of all beings in the game. Throws an instance of
// \Trogdord\NetworkException if the call fails due to network connectivity
// issues and an instance of \Trogdord\GameNotFound if the game has since been
// destroyed and no longer exists.
ZEND_BEGIN_ARG_INFO(arginfoListBeings, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Game, listBeings) {

}

/*****************************************************************************/

// Returns a list of all rooms in the game. Throws an instance of
// \Trogdord\NetworkException if the call fails due to network connectivity
// issues and an instance of \Trogdord\GameNotFound if the game has since been
// destroyed and no longer exists.
ZEND_BEGIN_ARG_INFO(arginfoListRooms, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Game, listRooms) {

}

/*****************************************************************************/

// Returns a list of all objects in the game. Throws an instance of
// \Trogdord\NetworkException if the call fails due to network connectivity
// issues and an instance of \Trogdord\GameNotFound if the game has since been
// destroyed and no longer exists.
ZEND_BEGIN_ARG_INFO(arginfoListObjects, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Game, listObjects) {

}

/*****************************************************************************/

// Returns a list of all creatures in the game. Throws an instance of
// \Trogdord\NetworkException if the call fails due to network connectivity
// issues and an instance of \Trogdord\GameNotFound if the game has since been
// destroyed and no longer exists.
ZEND_BEGIN_ARG_INFO(arginfoListCreatures, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Game, listCreatures) {

}

/*****************************************************************************/

// Returns a list of all players in the game. Throws an instance of
// \Trogdord\NetworkException if the call fails due to network connectivity
// issues and an instance of \Trogdord\GameNotFound if the game has since been
// destroyed and no longer exists.
ZEND_BEGIN_ARG_INFO(arginfoListPlayers, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Game, listPlayers) {

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

	zval *propVal, rv;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &nameLength) == FAILURE) {
		RETURN_NULL()
	}

	// TODO
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

	zval *propVal, rv;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &nameLength) == FAILURE) {
		RETURN_NULL()
	}

	// TODO
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

	zval *propVal, rv;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &nameLength) == FAILURE) {
		RETURN_NULL()
	}

	// TODO
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

	zval *propVal, rv;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &nameLength) == FAILURE) {
		RETURN_NULL()
	}

	// TODO
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

	zval *propVal, rv;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &nameLength) == FAILURE) {
		RETURN_NULL()
	}

	// TODO
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

	zval *propVal, rv;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &nameLength) == FAILURE) {
		RETURN_NULL()
	}

	// TODO
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

	zval *propVal, rv;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &nameLength) == FAILURE) {
		RETURN_NULL()
	}

	// TODO
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

	zval *propVal, rv;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &nameLength) == FAILURE) {
		RETURN_NULL()
	}

	// TODO
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
	PHP_ME(Game, listEntities, arginfoListEntities, ZEND_ACC_PUBLIC)
	PHP_ME(Game, listPlaces, arginfoListPlaces, ZEND_ACC_PUBLIC)
	PHP_ME(Game, listThings, arginfoListThings, ZEND_ACC_PUBLIC)
	PHP_ME(Game, listBeings, arginfoListBeings, ZEND_ACC_PUBLIC)
	PHP_ME(Game, listRooms, arginfoListRooms, ZEND_ACC_PUBLIC)
	PHP_ME(Game, listObjects, arginfoListObjects, ZEND_ACC_PUBLIC)
	PHP_ME(Game, listCreatures, arginfoListCreatures, ZEND_ACC_PUBLIC)
	PHP_ME(Game, listPlayers, arginfoListPlayers, ZEND_ACC_PUBLIC)
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
