#include <optional>

#include "json.h"
#include "request.h"
#include "utility.h"
#include "network/tcpconnectionmap.h"

#include "trogdord.h"
#include "game.h"
#include "phpexception.h"

#include "exception/networkexception.h"
#include "exception/requestexception.h"

#include "compatibility.h"

ZEND_DECLARE_MODULE_GLOBALS(trogdord);
ZEND_EXTERN_MODULE_GLOBALS(game);

zend_object_handlers trogdordObjectHandlers;

// This request retrieves statistics about the server and its environment
static const char *STATS_REQUEST = "{\"method\":\"get\",\"scope\":\"global\",\"action\":\"statistics\"}";

// This request retrieves a list of all existing games
static const char *GAME_LIST_REQUEST = "{\"method\":\"get\",\"scope\":\"game\",\"action\":\"list\"%meta}";

// This request retrieves a list of all available game definitions
static const char *DEF_LIST_REQUEST = "{\"method\":\"get\",\"scope\":\"game\",\"action\":\"definitions\"}";

// This request retrieves details of a specific game
static const char *GET_GAME_REQUEST = "{\"method\":\"get\",\"scope\":\"game\",\"args\":{\"id\":%gid}}";

// This request creates a new game
static const char *NEW_GAME_REQUEST = "{\"method\":\"post\",\"scope\":\"game\",\"args\":{\"name\":\"%name\",\"definition\":\"%definition\"%meta}}";

/*****************************************************************************/

// Trogdord Constructor (returned instance represents a connection to an
// instance of trogdord over the network.) Throws instance of
// \Trogdord\NetworkException if the attempt to connect is unsuccessful.
ZEND_BEGIN_ARG_INFO(arginfoCtor, 0)
	ZEND_ARG_TYPE_INFO(0, hostname, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, port, IS_LONG, 1)
ZEND_END_ARG_INFO()

PHP_METHOD(Trogdord, __construct) {

	trogdordObject *objWrapper = ZOBJ_TO_TROGDORD(Z_OBJ_P(getThis()));

	char *hostname;
	size_t hostnameLength;
	long port = TROGDORD_DEFAULT_PORT;

	zend_parse_parameters_throw(
		ZEND_NUM_ARGS() TSRMLS_CC,
		"s|l",
		&hostname,
		&hostnameLength,
		&port
	);

	objWrapper->data.hostname = hostname;
	objWrapper->data.port = port;

	try {
		TCPConnectionMap::get().connect(hostname, port);
	}

	// Throw \Trogord\NetworkException
	catch (const NetworkException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(networkException), e.what(), 0);
	}
}

/*****************************************************************************/

// Returns various server-specific statistics from an instance of trogdord.
// Throws an instance of \Trogdord\NetworkException if there's an issue with the
// network connection that prevents this call from returning valid data.
ZEND_BEGIN_ARG_INFO(arginfoStatistics, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Trogdord, statistics) {

	trogdordObject *objWrapper = ZOBJ_TO_TROGDORD(Z_OBJ_P(getThis()));

	ZEND_PARSE_PARAMETERS_NONE();

	try {

		JSONObject response = Request::execute(
			objWrapper->data.hostname,
			objWrapper->data.port,
			STATS_REQUEST
		);

		response.erase("status");
		zval data = JSON::JSONToZval(response);

		// There's some insanity in how this works, so for reference, here's
		// what I read to help me understand what all the arguments mean:
		// https://medium.com/@davidtstrauss/copy-and-move-semantics-of-zvals-in-php-7-41427223d784
		RETURN_ZVAL(&data, 1, 1);
	}

	// Throw \Trogord\NetworkException
	catch (const NetworkException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(networkException), e.what(), 0);
	}

	// Throw \Trogord\RequestException
	catch (const RequestException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(requestException), e.what(), e.getCode());
	}
}

/*****************************************************************************/

// Returns a list of all existing games in an instance of trogdord (could be an
// empty array if no games currently exist.) Takes as input an optional array of
// meta values that should be returned along with the id and name of each game.
// Throws an instance of \Trogdord\NetworkException if there's an issue with the
// network connection that prevents this call from returning a valid list.
ZEND_BEGIN_ARG_INFO(arginfoListGames, 0)
	ZEND_ARG_TYPE_INFO(0, keys, IS_ARRAY, 1)
ZEND_END_ARG_INFO()

PHP_METHOD(Trogdord, games) {

	zval *keys = nullptr;
	std::string metaArg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|a", &keys) == FAILURE) {
		RETURN_NULL();
	}

	if (nullptr != keys && zend_array_count(Z_ARRVAL_P(keys))) {

		zval *entry;
		HashPosition pos;

		bool firstEntryVisited = false;
		metaArg = ",\"args\":{\"include_meta\":[";

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

		metaArg += "]}";
	}

	trogdordObject *objWrapper = ZOBJ_TO_TROGDORD(Z_OBJ_P(getThis()));

	try {

		std::string request = GAME_LIST_REQUEST;
		strReplace(request, "%meta", metaArg);

		JSONObject response = Request::execute(
			objWrapper->data.hostname,
			objWrapper->data.port,
			request
		);

		zval data = JSON::JSONToZval(response.get_child("games"));

		// There's some insanity in how this works, so for reference, here's
		// what I read to help me understand what all the arguments mean:
		// https://medium.com/@davidtstrauss/copy-and-move-semantics-of-zvals-in-php-7-41427223d784
		RETURN_ZVAL(&data, 1, 1);
	}

	// Throw \Trogord\NetworkException
	catch (const NetworkException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(networkException), e.what(), 0);
	}

	// Throw \Trogord\RequestException
	catch (const RequestException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(requestException), e.what(), e.getCode());
	}
}

/*****************************************************************************/

// Returns a list of all game definitions available to an instance of trogdord
// (could be an empty array if no definitions are available.) Throws an instance
// of \Trogdord\NetworkException if there's an issue with the network connection
// that prevents this call from returning a valid list.
ZEND_BEGIN_ARG_INFO(arginfoListDefinitions, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Trogdord, definitions) {

	trogdordObject *objWrapper = ZOBJ_TO_TROGDORD(Z_OBJ_P(getThis()));

	ZEND_PARSE_PARAMETERS_NONE();

	try {

		JSONObject response = Request::execute(
			objWrapper->data.hostname,
			objWrapper->data.port,
			DEF_LIST_REQUEST
		);

		zval data = JSON::JSONToZval(response.get_child("definitions"));

		// There's some insanity in how this works, so for reference, here's
		// what I read to help me understand what all the arguments mean:
		// https://medium.com/@davidtstrauss/copy-and-move-semantics-of-zvals-in-php-7-41427223d784
		RETURN_ZVAL(&data, 1, 1);
	}

	// Throw \Trogord\NetworkException
	catch (const NetworkException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(networkException), e.what(), 0);
	}

	// Throw \Trogord\RequestException
	catch (const RequestException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(requestException), e.what(), e.getCode());
	}
}

/*****************************************************************************/

// Returns an instance of \Trogdord\Game, which provides an interface to a game
// that exists inside an instance of trogdord. Throws an instance of
// \Trogdord\GameNotFound if the game doesn't exist and
// \Trogdord\NetworkException if there's an issue with the network connection
// that prevents this call from returning a valid value.
ZEND_BEGIN_ARG_INFO(arginfoGetGame, 0)
	ZEND_ARG_TYPE_INFO(0, id, IS_LONG, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Trogdord, getGame) {

	size_t gameId;
	trogdordObject *objWrapper = ZOBJ_TO_TROGDORD(Z_OBJ_P(getThis()));

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &gameId)  == FAILURE) {
		RETURN_NULL();
	}

	try {

		std::string request = GET_GAME_REQUEST;
		strReplace(request, "%gid", std::to_string(gameId));

		JSONObject response = Request::execute(
			objWrapper->data.hostname,
			objWrapper->data.port,
			request
		);

		if (!createGameObj(return_value, response.get<std::string>("name"), gameId, getThis())) {
			php_error_docref(NULL, E_ERROR, "failed to instantiate Trogdord\\Game");
		}

		return;
	}

	// Throw \Trogord\NetworkException
	catch (const NetworkException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(networkException), e.what(), 0);
	}

	catch (const RequestException &e) {

		// Throw \Trogdord\GameNotFound
		if (404 == e.getCode()) {
			zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), e.getCode());
		}

		// Throw \Trogdord\RequestException
		else {
			zend_throw_exception(EXCEPTION_GLOBALS(requestException), e.what(), e.getCode());
		}
	}
}

/*****************************************************************************/

// Creates a new game inside an instance of trogdord and returns an instance of
// the PHP class \Trogdord\Game, which wraps around it. Takes an optional array
// argument representing a list of key => value meta data pairs that should be
// set on the game at the moment it's created. Throws \Trogdord\RequestException
// if there's an issue creating the game and \Trogdord\NetworkException if
// there's an issue with the network connection that prevents this call from
// creating a new game.
ZEND_BEGIN_ARG_INFO(arginfoNewGame, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, definition, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, meta, IS_ARRAY, 1)
ZEND_END_ARG_INFO()

PHP_METHOD(Trogdord, newGame) {

	char *name;
	size_t nameLength;

	char *definition;
	size_t definitionLength;

	zval *meta = nullptr;
	std::string metaArg;

	trogdordObject *objWrapper = ZOBJ_TO_TROGDORD(Z_OBJ_P(getThis()));

	if (zend_parse_parameters(
		ZEND_NUM_ARGS() TSRMLS_CC,
		"ss|a",
		&name,
		&nameLength,
		&definition,
		&definitionLength,
		&meta
	)  == FAILURE) {
		RETURN_NULL();
	}

	if (nullptr != meta && zend_array_count(Z_ARRVAL_P(meta))) {

		zval *entry;
		HashPosition pos;

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

			metaArg += std::string(",\"") + key + "\":\"" + Z_STRVAL_P(entry) + "\"";
		}
	}

	try {

		std::string request = NEW_GAME_REQUEST;
		strReplace(request, "%name", name);
		strReplace(request, "%definition", definition);
		strReplace(request, "%meta", metaArg);

		JSONObject response = Request::execute(
			objWrapper->data.hostname,
			objWrapper->data.port,
			request
		);

		if (!createGameObj(return_value, name, response.get<int>("id"), getThis())) {
			php_error_docref(NULL, E_ERROR, "failed to instantiate Trogdord\\Game");
		}

		return;
	}

	// Throw \Trogord\NetworkException
	catch (const NetworkException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(networkException), e.what(), 0);
	}

	// Throw \Trogdord\RequestException
	catch (const RequestException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(requestException), e.what(), e.getCode());
	}
}

/*****************************************************************************/

// PHP Trogdord class methods
static const zend_function_entry classMethods[] =  {
	PHP_ME(Trogdord, __construct, arginfoCtor, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(Trogdord, statistics, arginfoStatistics, ZEND_ACC_PUBLIC)
	PHP_ME(Trogdord, games, arginfoListGames, ZEND_ACC_PUBLIC)
	PHP_ME(Trogdord, definitions, arginfoListDefinitions, ZEND_ACC_PUBLIC)
	PHP_ME(Trogdord, getGame, arginfoGetGame, ZEND_ACC_PUBLIC)
	PHP_ME(Trogdord, newGame, arginfoNewGame, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/*****************************************************************************/
/*****************************************************************************/

// Custom Object Handlers
// See: http://blog.jpauli.tech/2016-01-14-php-7-objects-html/

static zend_object *createObject(zend_class_entry *classEntry TSRMLS_DC) {

	trogdordObject *obj = (trogdordObject *)ecalloc(1, sizeof(*obj) + zend_object_properties_size(classEntry));

	zend_object_std_init(&obj->std, classEntry);
	object_properties_init(&obj->std, classEntry);

	obj->std.handlers = &trogdordObjectHandlers;

	return &obj->std;
}

/*****************************************************************************/

static void destroyObject(zend_object *object TSRMLS_DC) {

	zend_objects_destroy_object(object);
}

/*****************************************************************************/

static void freeObject(zend_object *object TSRMLS_DC) {

	zend_object_std_dtor(object TSRMLS_CC);
}

/*****************************************************************************/
/*****************************************************************************/

void defineTrogdordClass() {

	zend_class_entry trogdordClass;

	INIT_CLASS_ENTRY(trogdordClass, "Trogdord", classMethods);
	TROGDORD_GLOBALS(classEntry) = zend_register_internal_class(&trogdordClass);

	// Make sure users can't extend the class
	TROGDORD_GLOBALS(classEntry)->ce_flags |= ZEND_ACC_FINAL;

	// Start out with default object handlers
	memcpy(
		&trogdordObjectHandlers,
		zend_get_std_object_handlers(),
		sizeof(trogdordObjectHandlers)
	);

	// Set the specific custom object handlers we need
	TROGDORD_GLOBALS(classEntry)->create_object = createObject;
	trogdordObjectHandlers.free_obj = freeObject;
	trogdordObjectHandlers.dtor_obj = destroyObject;

	// For an explanation of why this is necessary, see:
	// http://blog.jpauli.tech/2016-01-14-php-7-objects-html/
	trogdordObjectHandlers.offset = XtOffsetOf(trogdordObject, std);
}
