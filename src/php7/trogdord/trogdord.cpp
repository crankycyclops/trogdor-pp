#include <optional>

#include "trogdord.h"
#include "json.h"
#include "phpexception.h"
#include "network/tcpconnectionmap.h"
#include "exception/networkexception.h"

ZEND_DECLARE_MODULE_GLOBALS(trogdord);

zend_object_handlers trogdordObjectHandlers;

// This request retrieves statistics about the server and its environment
static const char *STATS_REQUEST = "{\"method\":\"get\",\"scope\":\"global\",\"action\":\"statistics\"}";

/*****************************************************************************/

// Utility method that sends a request to trogdord and processes the response.
static std::optional<JSONObject> makeRequest(zval *thisPtr, std::string request) {

	trogdordObject *objWrapper = ZOBJ_TO_TROGDORD(Z_OBJ_P(thisPtr));

	try {

		// As long as the connection wasn't closed at any point (either
		// intentionally or due to error), this should be fast and should only
		// retrieve an already opened and cached connection.
		TCPConnection &connection = TCPConnectionMap::get().connect(
			objWrapper->data.hostname,
			objWrapper->data.port
		);

		connection.write(request);
		std::string response = connection.read();

		JSONObject responseObj = JSON::deserialize(response);
		auto status = responseObj.get_optional<int>("status");

		if (!status || 200 != *status) {

			auto message = responseObj.get_optional<std::string>("message");

			zend_throw_exception(EXCEPTION_GLOBALS(
				requestException),
				message ? (*message).c_str() : "Invalid request",
				0
			);

			return std::nullopt;
		}

		return responseObj;
	}

	catch (boost::property_tree::json_parser::json_parser_error &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(networkException), "Invalid server response", 0);
		return std::nullopt;
	}

	catch (const NetworkException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(networkException), e.what(), 0);
		return std::nullopt;
	}
}

/*****************************************************************************/

// Trogdord Constructor (returned instance represents a connection to an
// instance of trogdord over the network.) Throws instance of
// \Trogdord\NetworkException if the attempt to connect is unsuccessful.
ZEND_BEGIN_ARG_INFO(arginfoCtor, 0)
	ZEND_ARG_TYPE_INFO(0, hostname, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, port, IS_LONG, 1)
ZEND_END_ARG_INFO()

PHP_METHOD(Trogdord, __construct) {

	zval *thisPtr = getThis();
	trogdordObject *objWrapper = ZOBJ_TO_TROGDORD(Z_OBJ_P(thisPtr));

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

	ZEND_PARSE_PARAMETERS_NONE();

	std::optional<JSONObject> response = makeRequest(getThis(), STATS_REQUEST);

	if (response.has_value()) {

		// TODO: extract values from JSONObject and return PHP array
		RETURN_STRING((*response).get<std::string>("status").c_str());
	}
}

/*****************************************************************************/

// Returns a list of all existing games in an instance of trogdord (could be an
// empty array if no games currently exist.) Throws an instance of
// \Trogdord\NetworkException if there's an issue with the network connection
// that prevents this call from returning a valid list.
ZEND_BEGIN_ARG_INFO(arginfoListGames, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Trogdord, listGames) {

	// Method takes no arguments
	ZEND_PARSE_PARAMETERS_NONE();

	// TODO
}

/*****************************************************************************/

// Returns a list of all game definitions available to an instance of trogdord
// (could be an empty array if no definitions are available.) Throws an instance
// of \Trogdord\NetworkException if there's an issue with the network connection
// that prevents this call from returning a valid list.
ZEND_BEGIN_ARG_INFO(arginfoListDefinitions, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Trogdord, listDefinitions) {

	// Method takes no arguments
	ZEND_PARSE_PARAMETERS_NONE();

	// TODO
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

	long gameId;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &gameId)  == FAILURE) {
		RETURN_NULL();
	}

	// TODO
	// Remember to set private property "id" with game's id
}

/*****************************************************************************/

// Creates a new game inside an instance of trogdord and returns an instance of
// the PHP class \Trogdord\Game, which wraps around it. Throws
// \Trogdord\RequestException if there's an issue creating the game and
// \Trogdord\NetworkException if there's an issue with the network connection
// that prevents this call from creating a new game.
ZEND_BEGIN_ARG_INFO(arginfoNewGame, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, definition, IS_STRING, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Trogdord, newGame) {

	char *name;
	size_t nameLength;

	char *definition;
	size_t definitionLength;

	if (zend_parse_parameters(
		ZEND_NUM_ARGS() TSRMLS_CC,
		"ss",
		&name,
		&nameLength,
		&definition,
		&definitionLength
	)  == FAILURE) {
		RETURN_NULL();
	}

	// TODO
	// Remember to set private property "id" with game's id
}

/*****************************************************************************/

// PHP Trogdord class methods
static const zend_function_entry classMethods[] =  {
	PHP_ME(Trogdord, __construct, arginfoCtor, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(Trogdord, statistics, arginfoStatistics, ZEND_ACC_PUBLIC)
	PHP_ME(Trogdord, listGames, arginfoListGames, ZEND_ACC_PUBLIC)
	PHP_ME(Trogdord, listDefinitions, arginfoListDefinitions, ZEND_ACC_PUBLIC)
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
