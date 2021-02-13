#include "entity.h"

#include "resource.h"
#include "room.h"
#include "object.h"
#include "creature.h"
#include "player.h"

#include "../game.h"
#include "../trogdord.h"
#include "../utility.h"
#include "../request.h"
#include "../json.h"

#include "../phpexception.h"
#include "../exception/requestexception.h"

ZEND_DECLARE_MODULE_GLOBALS(entity);

ZEND_EXTERN_MODULE_GLOBALS(trogdord);
ZEND_EXTERN_MODULE_GLOBALS(game);
ZEND_EXTERN_MODULE_GLOBALS(resource);
ZEND_EXTERN_MODULE_GLOBALS(room);
ZEND_EXTERN_MODULE_GLOBALS(object);
ZEND_EXTERN_MODULE_GLOBALS(creature);
ZEND_EXTERN_MODULE_GLOBALS(player);

// By default, we read from an entity's output buffer on this channel
static const char *ENTITY_DEFAULT_OUTPUT_CHANNEL = "notifications";

// This request returns all buffered output for the specified entity and channel.
static const char *ENTITY_GET_OUTPUT_REQUEST = "{\"method\":\"get\",\"scope\":\"entity\",\"action\":\"output\",\"args\":{\"game_id\":%gid,\"name\":\"%ename\",\"channel\":\"%channel\"}}";

// This request appends a string to the entity's output buffer on the specified
// channel.
static const char *ENTITY_POST_OUTPUT_REQUEST = "{\"method\":\"post\",\"scope\":\"entity\",\"action\":\"output\",\"args\":{\"game_id\":%gid,\"name\":\"%ename\",\"channel\":\"%channel\",\"message\":\"%output\"}}";

// Utility function that returns the contents of an entity's output buffer on
// the specified channel.
static zval getEntityOutput(zval *entityObj, std::string channel);

// Utility function that appends a message to the entity's output buffer on the
// specified channel.
static void appendToEntityOutput(zval *entityObj, std::string channel, std::string message);

/*****************************************************************************/

// The constructor should NEVER be called in PHP userland. Instead, instances of
// Trogdord\Entity or their inherited classes will be returned by calls to
// methods defined in \Trogdord\Game.
ZEND_BEGIN_ARG_INFO(arginfoCtor, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Entity, __construct) {

	php_error_docref(NULL, E_ERROR, "Tried to instantiate non-instantiable class.");
}

/*****************************************************************************/

// Magic "__get" allows us to make private and protected data members read-only.
ZEND_BEGIN_ARG_INFO(arginfoMagicGet, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Entity, __get) {

	char *key;
	size_t keyLength;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &key, &keyLength) == FAILURE) {
		RETURN_NULL();
	}

	zval rv; // ???
	zval *propVal = ENTITY_TO_PROP_VAL(getThis(), &rv, key);

	// There's some insanity in how this works, so for reference, here's what
	// I read to help me understand what all the arguments mean:
	// https://medium.com/@davidtstrauss/copy-and-move-semantics-of-zvals-in-php-7-41427223d784
	RETURN_ZVAL(propVal, 1, 0);
}

/*****************************************************************************/

// Depending on the number of arguments, either retrieves all messages currently
// in the entity's output buffer on the specified channel or sends a message to
// be appended to the output buffer on the specified channel. If neither a
// channel nor a message are specified in the arguments, the messages returned
// are those on the default channel.
ZEND_BEGIN_ARG_INFO_EX(arginfoOutput, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, channel, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 1)
ZEND_END_ARG_INFO()

PHP_METHOD(Entity, output) {

	zval rv; // ???

	char *channel = nullptr;
	size_t channelLength = 0;

	char *message = nullptr;
	size_t messageLength = 0;

	if (zend_parse_parameters(
		ZEND_NUM_ARGS(),
		"|ss",
		&channel,
		&channelLength,
		&message,
		&messageLength
	) == FAILURE) {
		RETURN_NULL();
	}

	ASSERT_GAME_IS_VALID(GAME_IS_VALID_PROP(ENTITY_TO_GAME(getThis(), &rv), &rv));
	ASSERT_ENTITY_IS_VALID(getThis());

	if (nullptr == channel || 0 == strlen(channel)) {
		channel = const_cast<char *>(ENTITY_DEFAULT_OUTPUT_CHANNEL);
	}

	try {

		// We're reading from the entity's output buffer
		if (nullptr == message) {
			zval output = getEntityOutput(getThis(), channel);
			RETURN_ZVAL(&output, 1, 1);
		}

		// We're appending a message to the entity's output buffer
		else {
			appendToEntityOutput(getThis(), channel, message);
			RETURN_NULL();
		}
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

// PHP Entity class methods
static const zend_function_entry entityMethods[] =  {
	PHP_ME(Entity, __construct, arginfoCtor, ZEND_ACC_PRIVATE | ZEND_ACC_CTOR)
	PHP_ME(Entity, __get, arginfoMagicGet, ZEND_ACC_PUBLIC)
	PHP_ME(Entity, output, arginfoOutput, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/*****************************************************************************/

static zval getEntityOutput(zval *entityObj, std::string channel) {

	zval rv; // ???
	zval *gameObj = ENTITY_TO_GAME(entityObj, &rv);

	std::string request = ENTITY_GET_OUTPUT_REQUEST;

	strReplace(request, "%gid", std::to_string(Z_LVAL_P(GAME_TO_ID(gameObj, &rv))));
	strReplace(request, "%ename", Z_STRVAL_P(ENTITY_TO_NAME(entityObj, &rv)));
	strReplace(request, "%channel", channel);

	trogdordObject *objWrapper = ZOBJ_TO_TROGDORD(Z_OBJ_P(GAME_TO_TROGDORD(gameObj, &rv)));

	Document response = Request::execute(
		objWrapper->data.hostname,
		objWrapper->data.port,
		request,
		GAME_TO_TROGDORD(gameObj, &rv)
	);

	return JSON::JSONToZval(response["messages"]);
}

/*****************************************************************************/

static void appendToEntityOutput(zval *entityObj, std::string channel, std::string message) {

	zval rv; // ???
	zval *gameObj = ENTITY_TO_GAME(entityObj, &rv);

	std::string request = ENTITY_POST_OUTPUT_REQUEST;

	strReplace(request, "%gid", std::to_string(Z_LVAL_P(GAME_TO_ID(gameObj, &rv))));
	strReplace(request, "%ename", Z_STRVAL_P(ENTITY_TO_NAME(entityObj, &rv)));
	strReplace(request, "%channel", channel);
	strReplace(request, "%output", message);

	trogdordObject *objWrapper = ZOBJ_TO_TROGDORD(Z_OBJ_P(GAME_TO_TROGDORD(gameObj, &rv)));

	Document response = Request::execute(
		objWrapper->data.hostname,
		objWrapper->data.port,
		request,
		GAME_TO_TROGDORD(gameObj, &rv)
	);
}

/*****************************************************************************/

bool createEntityObj(zval *entityObj, Value &properties, zval *gameObj) {

	zend_class_entry *eClassEntry;

	// My first *ever* use of this C++17 feature (I feel so grown up):
	// https://en.cppreference.com/w/cpp/language/if#If_Statements_with_Initializer
	// Ignore cppcheck's flagging of this as a syntax error
	if (
		std::string eType = properties["type"].GetString();
		0 == eType.compare(RESOURCE_TYPE_STR)
	) {
		eClassEntry = RESOURCE_GLOBALS(classEntry);
	}

	else if (0 == eType.compare(ROOM_TYPE_STR)) {
		eClassEntry = ROOM_GLOBALS(classEntry);
	}

	else if (0 == eType.compare(OBJECT_TYPE_STR)) {
		eClassEntry = OBJECT_GLOBALS(classEntry);
	}

	else if (0 == eType.compare(CREATURE_TYPE_STR)) {
		eClassEntry = CREATURE_GLOBALS(classEntry);
	}

	else if (0 == eType.compare(PLAYER_TYPE_STR)) {
		eClassEntry = PLAYER_GLOBALS(classEntry);
	}

	else {
		throw std::runtime_error("Entity type '" + eType + "' not recognized. This is a bug.");
	}

	if (SUCCESS != object_init_ex(entityObj, eClassEntry)) {
		return false;
	}

	zend_update_property_string(
		eClassEntry,
		#if ZEND_MODULE_API_NO >= 20200930 // PHP 8.0+
			Z_OBJ_P(entityObj),
		#else
			entityObj,
		#endif
		NAME_PROPERTY_NAME,
		strlen(NAME_PROPERTY_NAME),
		properties["name"].GetString()
	);

	zend_update_property(
		eClassEntry,
		#if ZEND_MODULE_API_NO >= 20200930 // PHP 8.0+
			Z_OBJ_P(entityObj),
		#else
			entityObj,
		#endif
		GAME_PROPERTY_NAME,
		strlen(GAME_PROPERTY_NAME),
		gameObj
	);

	zend_update_property_bool(
		eClassEntry,
		#if ZEND_MODULE_API_NO >= 20200930 // PHP 8.0+
			Z_OBJ_P(entityObj),
		#else
			entityObj,
		#endif
		ENTITY_VALID_PROPERTY,
		strlen(ENTITY_VALID_PROPERTY),
		1
	);

	return true;
}

/*****************************************************************************/

void defineEntityClass() {

	zend_class_entry entityClass;

	INIT_CLASS_ENTRY(entityClass, "Trogdord\\Entity", entityMethods);
	ENTITY_GLOBALS(classEntry) = zend_register_internal_class(&entityClass);
	ENTITY_GLOBALS(classEntry)->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;

	// Declare the Entity class's properties
	zend_declare_property_null(
		ENTITY_GLOBALS(classEntry),
		NAME_PROPERTY_NAME,
		strlen(NAME_PROPERTY_NAME),
		ZEND_ACC_PROTECTED
	);

	zend_declare_property_null(
		ENTITY_GLOBALS(classEntry),
		GAME_PROPERTY_NAME,
		strlen(GAME_PROPERTY_NAME),
		ZEND_ACC_PROTECTED
	);

	zend_declare_property_bool(
		ENTITY_GLOBALS(classEntry),
		ENTITY_VALID_PROPERTY,
		strlen(ENTITY_VALID_PROPERTY),
		0,
		ZEND_ACC_PROTECTED
	);
}
