#include <Zend/zend_long.h>

#include "../trogdord.h"
#include "../json.h"
#include "../request.h"
#include "../utility.h"

#include "../phpexception.h"
#include "../exception/requestexception.h"

#include "dump.h"

ZEND_DECLARE_MODULE_GLOBALS(dump);
ZEND_EXTERN_MODULE_GLOBALS(trogdord);

// This request retrieves a list of all slots for a dumped game
static const char *DUMP_LIST_REQUEST = "{\"method\":\"get\",\"scope\":\"game\",\"action\":\"dumplist\",\"args\":{\"id\":%gid}}";

/*****************************************************************************/

// The constructor should NEVER be called in PHP userland. Instead, instances of
// Trogdord\Game\Dump will be returned by calls to \Trogdord::getDump().
ZEND_BEGIN_ARG_INFO(arginfoCtor, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Dump, __construct) {

	php_error_docref(NULL, E_ERROR, "Tried to instantiate non-instantiable class.");
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

	size_t slot;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &slot)  == FAILURE) {
		RETURN_NULL();
	}

	// TODO
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
	ASSERT_DUMP_ID_IS_VALID(Z_TYPE_P(id));

	try {

		std::string request = DUMP_LIST_REQUEST;
		strReplace(request, "%gid", std::to_string(Z_LVAL_P(id)));

		trogdordObject *objWrapper = ZOBJ_TO_TROGDORD(Z_OBJ_P(trogdord));

		Document response = Request::execute(
			objWrapper->data.hostname,
			objWrapper->data.port,
			request,
			getThis()
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

// Destroys the game's entire dump history, making it permanently inaccessible.
// Throws an instance of \Trogdord\NetworkException if the call fails due to
// network connectivity issues. If the game has already been destroyed,
// \Trogdord\GameNotFound will be thrown.
ZEND_BEGIN_ARG_INFO(arginfoDestroy, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Dump, destroy) {

	// TODO
}

/*****************************************************************************/

// PHP Dump class methods
static const zend_function_entry classMethods[] =  {
	PHP_ME(Dump, __construct, arginfoCtor, ZEND_ACC_PRIVATE | ZEND_ACC_CTOR)
	PHP_ME(Dump, getSlot, arginfoGetSlot, ZEND_ACC_PUBLIC)
	PHP_ME(Dump, slots, arginfoSlots, ZEND_ACC_PUBLIC)
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

	return true;
}

/*****************************************************************************/

void defineDumpClass() {

	zend_class_entry dumpClass;

	INIT_CLASS_ENTRY(dumpClass, "Trogdord\\Game\\Dump", classMethods);
	DUMP_GLOBALS(classEntry) = zend_register_internal_class(&dumpClass);

	zend_declare_property_null(
		DUMP_GLOBALS(classEntry),
		DUMP_TROGDORD_PROPERTY,
		strlen(DUMP_TROGDORD_PROPERTY),
		ZEND_ACC_PRIVATE
	);

	// Make sure users can't extend the class
	DUMP_GLOBALS(classEntry)->ce_flags |= ZEND_ACC_FINAL;
}
