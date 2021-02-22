#include "../trogdord.h"
#include "../json.h"
#include "../request.h"
#include "../utility.h"

#include "../game.h"
#include "../phpexception.h"
#include "../exception/requestexception.h"

#include "dump.h"
#include "slot.h"

// This must be included after "dump.h" because it depends on "php_config.h"
#include <Zend/zend_long.h>

ZEND_DECLARE_MODULE_GLOBALS(dump);
ZEND_EXTERN_MODULE_GLOBALS(trogdord);

// This request restores a specific game dump
static const char *DUMP_RESTORE_REQUEST = "{\"method\":\"post\",\"scope\":\"game\",\"action\":\"restore\",\"args\":{\"id\":%gid%slotArg}}";

// This request retrieves a specific dump slot
static const char *DUMP_GET_SLOT_REQUEST = "{\"method\":\"get\",\"scope\":\"game\",\"action\":\"dump\",\"args\":{\"id\":%gid,\"slot\":%slot}}";

// This request retrieves a list of all slots for a dumped game
static const char *DUMP_LIST_REQUEST = "{\"method\":\"get\",\"scope\":\"game\",\"action\":\"dumplist\",\"args\":{\"id\":%gid}}";

// This request destroys a game's entire dump history
static const char *DUMP_DESTROY_REQUEST = "{\"method\":\"delete\",\"scope\":\"game\",\"action\":\"dump\",\"args\":{\"id\":%gid}}";

/*****************************************************************************/

// The constructor should NEVER be called in PHP userland. Instead, instances of
// Trogdord\Game\Dump will be returned by calls to \Trogdord::getDump().
ZEND_BEGIN_ARG_INFO(arginfoCtor, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Dump, __construct) {

	php_error_docref(NULL, E_ERROR, "Tried to instantiate non-instantiable class.");
}

/*****************************************************************************/

// Magic "__get" allows us to make private and protected data members read-only.
ZEND_BEGIN_ARG_INFO(arginfoMagicGet, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Dump, __get) {

	char *key;
	size_t keyLength;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &key, &keyLength) == FAILURE) {
		RETURN_NULL();
	}

	zval rv; // ???
	zval *propVal = DUMP_TO_PROP_VAL(getThis(), &rv, key);

	// There's some insanity in how this works, so for reference, here's what
	// I read to help me understand what all the arguments mean:
	// https://medium.com/@davidtstrauss/copy-and-move-semantics-of-zvals-in-php-7-41427223d784
	RETURN_ZVAL(propVal, 1, 0);
}

/*****************************************************************************/

// Returns an instance of \Trogdord\Game\Dump\Slot, which provides an interface
// to a game dump's slot. Throws an instance of \Trogdord\GameNotFound if the
// slot doesn't exist and \Trogdord\NetworkException if there's an issue with
// the network connection that prevents this call from returning a valid value.
ZEND_BEGIN_ARG_INFO(arginfoGetSlot, 0)
	ZEND_ARG_TYPE_INFO(0, id, IS_LONG, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Dump, getSlot) {

	zval rv; // ???

	size_t slot;
	zval *id = DUMP_TO_ID(getThis(), &rv);
	zval *trogdord = DUMP_TO_TROGDORD(getThis(), &rv);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &slot)  == FAILURE) {
		RETURN_NULL();
	}

	ASSERT_DUMP_IS_VALID(DUMP_IS_VALID_PROP(getThis(), &rv));

	try {

		std::string request = DUMP_GET_SLOT_REQUEST;
		strReplace(request, "%gid", std::to_string(Z_LVAL_P(id)));
		strReplace(request, "%slot", std::to_string(slot));

		trogdordObject *objWrapper = ZOBJ_TO_TROGDORD(Z_OBJ_P(trogdord));

		Document response = Request::execute(
			objWrapper->data.hostname,
			objWrapper->data.port,
			request,
			trogdord
		);

		if (!createDumpSlotObj(
			return_value,
			slot,
			#ifdef ZEND_ENABLE_ZVAL_LONG64
				response["timestamp_ms"].GetUint64(),
			#else
				response["timestamp_ms"].GetUint(),
			#endif
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

		// Throw \Trogdord\DumpSlotNotFound
		if (404 == e.getCode()) {
			zend_throw_exception(EXCEPTION_GLOBALS(dumpSlotNotFound), e.what(), e.getCode());
		}

		// Throw \Trogdord\RequestException
		else {
			zend_throw_exception(EXCEPTION_GLOBALS(requestException), e.what(), e.getCode());
		}
	}
}

/*****************************************************************************/

// Returns a list of all slots in the game's dump history. Throws an instance
// of \Trogdord\NetworkException if there's an issue with the network connection
// that prevents this call from returning a valid list.
ZEND_BEGIN_ARG_INFO(arginfoSlots, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Dump, slots) {

	zval rv; // ???

	zval *trogdord = DUMP_TO_TROGDORD(getThis(), &rv);
	zval *id = DUMP_TO_ID(getThis(), &rv);

	ZEND_PARSE_PARAMETERS_NONE();
	ASSERT_DUMP_IS_VALID(DUMP_IS_VALID_PROP(getThis(), &rv));

	try {

		std::string request = DUMP_LIST_REQUEST;
		strReplace(request, "%gid", std::to_string(Z_LVAL_P(id)));

		trogdordObject *objWrapper = ZOBJ_TO_TROGDORD(Z_OBJ_P(trogdord));

		Document response = Request::execute(
			objWrapper->data.hostname,
			objWrapper->data.port,
			request,
			trogdord
		);

		zval data = JSON::JSONToZval(response["slots"]);

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

// Restores the game dump. Optional argument is the slot that should be restored.
// If that argument is not present, the most recent slot is restored. Returns an
// instance of \Trogdord\Game representing the newly restored game. Throws an
// instance of \Trogdord\GameNotFound if the dump no longer exists, an instance
// of \Trogdord\DumpSlotNotFound if the specified slot doesn't exist, and
// \Trogdord\NetworkException if there's an issue with the network connection
// that prevents this call from returning a valid value.
ZEND_BEGIN_ARG_INFO_EX(arginfoRestore, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, slot, IS_LONG, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Dump, restore) {

	zval rv; // ???

	zval *trogdord = DUMP_TO_TROGDORD(getThis(), &rv);
	zval *id = DUMP_TO_ID(getThis(), &rv);

	zend_long slot = -1;

	ASSERT_DUMP_IS_VALID(DUMP_IS_VALID_PROP(getThis(), &rv));

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l", &slot) == FAILURE) {
		RETURN_NULL();
	}

	std::string slotArg = "";

	if (slot >= 0) {
		slotArg = std::string(",\"slot\":") + std::to_string(slot);
	}

	try {

		std::string request = DUMP_RESTORE_REQUEST;
		strReplace(request, "%gid", std::to_string(Z_LVAL_P(id)));
		strReplace(request, "%slotArg", slotArg);

		trogdordObject *objWrapper = ZOBJ_TO_TROGDORD(Z_OBJ_P(trogdord));

		Document response = Request::execute(
			objWrapper->data.hostname,
			objWrapper->data.port,
			request,
			trogdord
		);

		zval *created = DUMP_TO_PROP_VAL(getThis(), &rv, DUMP_CREATED_PROPERTY);

		if (!createGameObj(
			return_value,
			ZSTR_VAL(Z_STR_P(DUMP_TO_PROP_VAL(getThis(), &rv, DUMP_NAME_PROPERTY))),
			ZSTR_VAL(Z_STR_P(DUMP_TO_PROP_VAL(getThis(), &rv, DUMP_DEFINITION_PROPERTY))),
			IS_LONG == Z_TYPE_P(created) ? Z_LVAL_P(created) : Z_DVAL_P(created),
			IS_LONG == Z_TYPE_P(id) ? Z_LVAL_P(id) : Z_DVAL_P(id),
			trogdord
		)) {
			php_error_docref(NULL, E_ERROR, "failed to instantiate Trogdord\\Game");
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

// Destroys the game's entire dump history, making it permanently inaccessible.
// Throws an instance of \Trogdord\NetworkException if the call fails due to
// network connectivity issues. If the game has already been destroyed,
// \Trogdord\GameNotFound will be thrown.
ZEND_BEGIN_ARG_INFO(arginfoDestroy, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Dump, destroy) {

	zval rv; // ???

	zval *trogdord = DUMP_TO_TROGDORD(getThis(), &rv);
	zval *id = DUMP_TO_ID(getThis(), &rv);

	ZEND_PARSE_PARAMETERS_NONE();
	ASSERT_DUMP_IS_VALID(DUMP_IS_VALID_PROP(getThis(), &rv));

	try {

		std::string request = DUMP_DESTROY_REQUEST;
		strReplace(request, "%gid", std::to_string(Z_LVAL_P(id)));

		trogdordObject *objWrapper = ZOBJ_TO_TROGDORD(Z_OBJ_P(trogdord));

		Document response = Request::execute(
			objWrapper->data.hostname,
			objWrapper->data.port,
			request,
			trogdord
		);

		// Invalidate the object so it can't be used anymore
		zend_update_property_bool(
			DUMP_GLOBALS(classEntry),
			#if ZEND_MODULE_API_NO >= 20200930 // PHP 8.0+
				Z_OBJ_P(getThis()),
			#else
				getThis(),
			#endif
			DUMP_VALID_PROPERTY,
			strlen(DUMP_VALID_PROPERTY),
			0
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

// PHP Dump class methods
static const zend_function_entry classMethods[] =  {
	PHP_ME(Dump, __construct, arginfoCtor, ZEND_ACC_PRIVATE | ZEND_ACC_CTOR)
	PHP_ME(Dump, __get, arginfoMagicGet, ZEND_ACC_PUBLIC)
	PHP_ME(Dump, getSlot, arginfoGetSlot, ZEND_ACC_PUBLIC)
	PHP_ME(Dump, slots, arginfoSlots, ZEND_ACC_PUBLIC)
	PHP_ME(Dump, restore, arginfoRestore, ZEND_ACC_PUBLIC)
	PHP_ME(Dump, destroy, arginfoDestroy, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/*****************************************************************************/

bool createDumpObj(
	zval *dumpObj,
	size_t id,
	std::string name,
	std::string definition,
	size_t created,
	zval *trogdordObj
) {

	if (SUCCESS != object_init_ex(dumpObj, DUMP_GLOBALS(classEntry))) {
		return false;
	}

	if (id > ZEND_LONG_MAX) {
		zend_update_property_double(
			DUMP_GLOBALS(classEntry),
			#if ZEND_MODULE_API_NO >= 20200930 // PHP 8.0+
				Z_OBJ_P(dumpObj),
			#else
				dumpObj,
			#endif
			DUMP_ID_PROPERTY,
			strlen(DUMP_ID_PROPERTY),
			id
		);
	} else {
		zend_update_property_long(
			DUMP_GLOBALS(classEntry),
			#if ZEND_MODULE_API_NO >= 20200930 // PHP 8.0+
				Z_OBJ_P(dumpObj),
			#else
				dumpObj,
			#endif
			DUMP_ID_PROPERTY,
			strlen(DUMP_ID_PROPERTY),
			id
		);
	}

	if (created > ZEND_LONG_MAX) {
		zend_update_property_double(
			DUMP_GLOBALS(classEntry),
			#if ZEND_MODULE_API_NO >= 20200930 // PHP 8.0+
				Z_OBJ_P(dumpObj),
			#else
				dumpObj,
			#endif
			DUMP_CREATED_PROPERTY,
			strlen(DUMP_CREATED_PROPERTY),
			created
		);
	} else {
		zend_update_property_long(
			DUMP_GLOBALS(classEntry),
			#if ZEND_MODULE_API_NO >= 20200930 // PHP 8.0+
				Z_OBJ_P(dumpObj),
			#else
				dumpObj,
			#endif
			DUMP_CREATED_PROPERTY,
			strlen(DUMP_CREATED_PROPERTY),
			created
		);
	}

	zend_update_property_string(
		DUMP_GLOBALS(classEntry),
		#if ZEND_MODULE_API_NO >= 20200930 // PHP 8.0+
			Z_OBJ_P(dumpObj),
		#else
			dumpObj,
		#endif
		DUMP_NAME_PROPERTY,
		strlen(DUMP_NAME_PROPERTY),
		name.c_str()
	);

	zend_update_property_string(
		DUMP_GLOBALS(classEntry),
		#if ZEND_MODULE_API_NO >= 20200930 // PHP 8.0+
			Z_OBJ_P(dumpObj),
		#else
			dumpObj,
		#endif
		DUMP_DEFINITION_PROPERTY,
		strlen(DUMP_DEFINITION_PROPERTY),
		definition.c_str()
	);

	zend_update_property(
		DUMP_GLOBALS(classEntry),
		#if ZEND_MODULE_API_NO >= 20200930 // PHP 8.0+
			Z_OBJ_P(dumpObj),
		#else
			dumpObj,
		#endif
		DUMP_TROGDORD_PROPERTY,
		strlen(DUMP_TROGDORD_PROPERTY),
		trogdordObj
	);

	zend_update_property_bool(
		DUMP_GLOBALS(classEntry),
		#if ZEND_MODULE_API_NO >= 20200930 // PHP 8.0+
			Z_OBJ_P(dumpObj),
		#else
			dumpObj,
		#endif
		DUMP_VALID_PROPERTY,
		strlen(DUMP_VALID_PROPERTY),
		1
	);

	return true;
}

/*****************************************************************************/

void defineDumpClass() {

	zend_class_entry dumpClass;

	INIT_CLASS_ENTRY(dumpClass, "Trogdord\\Game\\Dump", classMethods);
	DUMP_GLOBALS(classEntry) = zend_register_internal_class(&dumpClass);

	zend_declare_property_null(
		DUMP_GLOBALS(classEntry),
		DUMP_ID_PROPERTY,
		strlen(DUMP_ID_PROPERTY),
		ZEND_ACC_PRIVATE
	);

	zend_declare_property_null(
		DUMP_GLOBALS(classEntry),
		DUMP_NAME_PROPERTY,
		strlen(DUMP_NAME_PROPERTY),
		ZEND_ACC_PRIVATE
	);

	zend_declare_property_null(
		DUMP_GLOBALS(classEntry),
		DUMP_DEFINITION_PROPERTY,
		strlen(DUMP_DEFINITION_PROPERTY),
		ZEND_ACC_PRIVATE
	);

	zend_declare_property_null(
		DUMP_GLOBALS(classEntry),
		DUMP_CREATED_PROPERTY,
		strlen(DUMP_CREATED_PROPERTY),
		ZEND_ACC_PRIVATE
	);

	zend_declare_property_null(
		DUMP_GLOBALS(classEntry),
		DUMP_TROGDORD_PROPERTY,
		strlen(DUMP_TROGDORD_PROPERTY),
		ZEND_ACC_PRIVATE
	);

	zend_declare_property_bool(
		DUMP_GLOBALS(classEntry),
		DUMP_VALID_PROPERTY,
		strlen(DUMP_VALID_PROPERTY),
		0,
		ZEND_ACC_PRIVATE
	);

	// Make sure users can't extend the class
	DUMP_GLOBALS(classEntry)->ce_flags |= ZEND_ACC_FINAL;
}
