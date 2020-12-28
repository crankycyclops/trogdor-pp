#include <optional>

#include "json.h"
#include "request.h"
#include "utility.h"
#include "network/tcpconnectionmap.h"

#include "trogdord.h"
#include "game.h"
#include "phpexception.h"

#include "exception/jsonexception.h"
#include "exception/networkexception.h"
#include "exception/requestexception.h"

#include "compatibility.h"

ZEND_DECLARE_MODULE_GLOBALS(trogdord);
ZEND_EXTERN_MODULE_GLOBALS(game);

zend_object_handlers trogdordObjectHandlers;

// This request retrieves statistics about the server and its environment
static const char *STATS_REQUEST = "{\"method\":\"get\",\"scope\":\"global\",\"action\":\"statistics\"}";

// This request dumps the server's state to disk
static const char *DUMP_REQUEST = "{\"method\":\"post\",\"scope\":\"global\",\"action\":\"dump\"}";

// This request restores the server's state from disk
static const char *RESTORE_REQUEST = "{\"method\":\"post\",\"scope\":\"global\",\"action\":\"restore\"}";

// This request retrieves a list of all existing games
static const char *GAME_LIST_REQUEST = "{\"method\":\"get\",\"scope\":\"game\",\"action\":\"list\"%args}";

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

		Document response = Request::execute(
			objWrapper->data.hostname,
			objWrapper->data.port,
			STATS_REQUEST,
			getThis()
		);

		response.RemoveMember("status");
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

// Dumps the server's state to disk, including all games. Throws an instance of
// \Trogdord\NetworkException if there's an issue with the network connection
// that prevents this call from returning valid data.
ZEND_BEGIN_ARG_INFO(arginfoDump, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Trogdord, dump) {

	trogdordObject *objWrapper = ZOBJ_TO_TROGDORD(Z_OBJ_P(getThis()));

	ZEND_PARSE_PARAMETERS_NONE();

	try {

		Document response = Request::execute(
			objWrapper->data.hostname,
			objWrapper->data.port,
			DUMP_REQUEST,
			getThis()
		);

		RETURN_NULL();
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

// Restores the server's state from disk, including all games. Throws an
// instance of \Trogdord\NetworkException if there's an issue with the network
// connection that prevents this call from returning valid data.
ZEND_BEGIN_ARG_INFO(arginfoRestore, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Trogdord, restore) {

	trogdordObject *objWrapper = ZOBJ_TO_TROGDORD(Z_OBJ_P(getThis()));

	ZEND_PARSE_PARAMETERS_NONE();

	try {

		Document response = Request::execute(
			objWrapper->data.hostname,
			objWrapper->data.port,
			RESTORE_REQUEST,
			getThis()
		);

		RETURN_NULL();
	}

	// Throw \Trogord\NetworkException
	catch (const NetworkException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(networkException), e.what(), 0);
	}

	// Throw \Trogord\RequestException
	catch (const RequestException &e) {
		if (206 == e.getCode()) {
			RETURN_NULL();
		} else {
			zend_throw_exception(EXCEPTION_GLOBALS(requestException), e.what(), e.getCode());
		}
	}
}

/*****************************************************************************/

// Returns a list of all existing games in an instance of trogdord (could be an
// empty array if no games currently exist.) Takes as input an optional array of
// meta values that should be returned along with the id and name of each game.
// Throws an instance of \Trogdord\NetworkException if there's an issue with the
// network connection that prevents this call from returning a valid list.
ZEND_BEGIN_ARG_INFO(arginfoListGames, 0)
	ZEND_ARG_TYPE_INFO(0, filters, IS_ARRAY, 1)
	ZEND_ARG_TYPE_INFO(0, keys, IS_ARRAY, 1)
ZEND_END_ARG_INFO()

PHP_METHOD(Trogdord, games) {

	zval *filters = nullptr;
	zval *keys = nullptr;

	std::string args;
	std::string filterArg;
	std::string metaArg;

	// The ! character tells us that if we pass in a literal null for the first
	// argument, filters will be set to a nullptr rather than us getting a
	// warning about the first parameter not being of the correct type.
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|a!a", &filters, &keys) == FAILURE) {
		RETURN_NULL();
	}

	// We're only returning games matching specific criteria
	if (nullptr != filters && zend_array_count(Z_ARRVAL_P(filters))) {

		try {
			filterArg = std::string("\"filters\":") + JSON::serialize(JSON::ZvalToJSON(filters));
		}

		catch (const JSONException &e) {
			zend_throw_exception(EXCEPTION_GLOBALS(filterException), e.what(), 0);
		}
	}

	// We need to include meta values in the list of returned games
	if (nullptr != keys && zend_array_count(Z_ARRVAL_P(keys))) {

		zval *entry;
		HashPosition pos;

		bool firstEntryVisited = false;
		metaArg = "\"include_meta\":[";

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

	if (filterArg.length() || metaArg.length()) {

		args = ",\"args\":{";

		if (filterArg.length()) {
			args += filterArg;
		}

		if (metaArg.length()) {
			args += filterArg.length() ? "," : "";
			args += metaArg;
		}

		args += "}";
	}

	trogdordObject *objWrapper = ZOBJ_TO_TROGDORD(Z_OBJ_P(getThis()));

	try {

		std::string request = GAME_LIST_REQUEST;
		strReplace(request, "%args", args);

		Document response = Request::execute(
			objWrapper->data.hostname,
			objWrapper->data.port,
			request,
			getThis()
		);

		zval data = JSON::JSONToZval(response["games"]);

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

		Document response = Request::execute(
			objWrapper->data.hostname,
			objWrapper->data.port,
			DEF_LIST_REQUEST,
			getThis()
		);

		zval data = JSON::JSONToZval(response["definitions"]);

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

		Document response = Request::execute(
			objWrapper->data.hostname,
			objWrapper->data.port,
			request,
			getThis()
		);

		if (!createGameObj(
			return_value,
			response["name"].GetString(),
			response["definition"].GetString(),
			gameId,
			getThis()
		)) {
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

		Document response = Request::execute(
			objWrapper->data.hostname,
			objWrapper->data.port,
			request,
			getThis()
		);

		#ifdef ZEND_ENABLE_ZVAL_LONG64
			int64_t id = response["id"].GetInt64();
		#else
			int id = response["id"].GetInt();
		#endif

		if (!createGameObj(return_value, name, definition, id, getThis())) {
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
	PHP_ME(Trogdord, dump, arginfoDump, ZEND_ACC_PUBLIC)
	PHP_ME(Trogdord, restore, arginfoRestore, ZEND_ACC_PUBLIC)
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

	trogdordObject *obj = static_cast<trogdordObject *>(
		ecalloc(1, sizeof(*obj) + zend_object_properties_size(classEntry))
	);

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

// Danger, Will Robinson! The arguments here changed between 7.4 and 8.0, so
// I'll have to conditionally define this function. See definition of
// mysqli_write_property in mysqli's source code for an example of how this
// differs.
static WRITE_PROP_RETURN_TYPE writeObjectProperty(zval *object, zval *member, zval *value, void **cache_slot) {

	// The status code of whatever the last request was is a read-only property
	if (
		Z_TYPE_P(member) == IS_STRING &&
		0 == strncmp(Z_STRVAL_P(member), STATUS_PROPERTY, Z_STRLEN_P(member))
	) {
		zend_throw_error(NULL, "Cannot write property");
	}

	else {
		#if ZEND_MODULE_API_NO >= 20190902 // PHP 7.4+
			value = zend_std_write_property(object, member, value, cache_slot);
		#else
			zend_std_write_property(object, member, value, cache_slot);
		#endif
	}

	#if ZEND_MODULE_API_NO >= 20190902 // PHP 7.4+
		return value;
	#endif
}

/*****************************************************************************/
/*****************************************************************************/

void defineTrogdordClass() {

	zend_class_entry trogdordClass;

	INIT_CLASS_ENTRY(trogdordClass, "Trogdord", classMethods);
	TROGDORD_GLOBALS(classEntry) = zend_register_internal_class(&trogdordClass);

	// Whenever a request takes place, this property gets set to response code
	// returned as part of the response
	zend_declare_property_null(
		TROGDORD_GLOBALS(classEntry),
		STATUS_PROPERTY,
		strlen(STATUS_PROPERTY),
		ZEND_ACC_PUBLIC
		TSRMLS_CC
	);

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
	trogdordObjectHandlers.write_property = writeObjectProperty;

	// For an explanation of why this is necessary, see:
	// http://blog.jpauli.tech/2016-01-14-php-7-objects-html/
	trogdordObjectHandlers.offset = XtOffsetOf(trogdordObject, std);
}
