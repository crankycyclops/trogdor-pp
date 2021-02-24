#include "../trogdord.h"
#include "../json.h"
#include "../request.h"
#include "../utility.h"

#include "../game.h"
#include "../phpexception.h"
#include "../exception/requestexception.h"

#include "dump.h"
#include "slot.h"

ZEND_DECLARE_MODULE_GLOBALS(slot);
ZEND_EXTERN_MODULE_GLOBALS(dump);

// This request restores a specific game dump slot
static const char *SLOT_RESTORE_REQUEST = "{\"method\":\"post\",\"scope\":\"game\",\"action\":\"restore\",\"args\":{\"id\":%gid,\"slot\":%slot}}";

// This request destroys a dump slot
static const char *SLOT_DESTROY_REQUEST = "{\"method\":\"delete\",\"scope\":\"game\",\"action\":\"dump\",\"args\":{\"id\":%gid,\"slot\":%slot}}";

/*****************************************************************************/

// The constructor should NEVER be called in PHP userland. Instead, instances of
// Trogdord\Game\Dump\Slot will be returned by calls to \Trogdord\Game\Dump::getSlot().
ZEND_BEGIN_ARG_INFO(arginfoCtor, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Slot, __construct) {

	php_error_docref(NULL, E_ERROR, "Tried to instantiate non-instantiable class.");
}

/*****************************************************************************/

// Magic "__get" allows us to make private and protected data members read-only.
ZEND_BEGIN_ARG_INFO(arginfoMagicGet, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Slot, __get) {

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

// Equivalent to a call to \Trogdord\Game\Dump::restore() with the optional
// slot argment. Throws an instance of \Trogdord\DumpSlotNotFound if the slot
// no longer exists, an instance of \Trogdord\GameNotFound if the dump the slot
// belongs to no longer exists, and \Trogdord\NetworkException if there's an
// issue with the network connection that prevents this call from returning a
// valid value.
ZEND_BEGIN_ARG_INFO(arginfoRestore, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Slot, restore) {

	zval rv; // ???

	zval *dump = SLOT_TO_DUMP(getThis(), &rv);
	zval *trogdord = DUMP_TO_TROGDORD(dump, &rv);
	zval *id = DUMP_TO_ID(dump, &rv);
	zval *slot = SLOT_TO_SLOTVAL(getThis(), &rv);

	ZEND_PARSE_PARAMETERS_NONE();
	ASSERT_DUMP_IS_VALID(DUMP_IS_VALID_PROP(dump, &rv));
	ASSERT_DUMP_SLOT_IS_VALID(SLOT_IS_VALID_PROP(getThis(), &rv));

	try {

		std::string request = SLOT_RESTORE_REQUEST;

		strReplace(request, "%gid", std::to_string(
			IS_LONG == Z_TYPE_P(id) ? Z_LVAL_P(id) : static_cast<int>(Z_DVAL_P(id))
		));
		strReplace(request, "%slot", std::to_string(
			IS_LONG == Z_TYPE_P(slot) ? Z_LVAL_P(slot) : static_cast<int>(Z_DVAL_P(slot))
		));

		trogdordObject *objWrapper = ZOBJ_TO_TROGDORD(Z_OBJ_P(trogdord));

		Document response = Request::execute(
			objWrapper->data.hostname,
			objWrapper->data.port,
			request,
			trogdord
		);

		zval *created = DUMP_TO_PROP_VAL(dump, &rv, DUMP_CREATED_PROPERTY);

		if (!createGameObj(
			return_value,
			ZSTR_VAL(Z_STR_P(DUMP_TO_PROP_VAL(dump, &rv, DUMP_NAME_PROPERTY))),
			ZSTR_VAL(Z_STR_P(DUMP_TO_PROP_VAL(dump, &rv, DUMP_DEFINITION_PROPERTY))),
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

		if (404 == e.getCode()) {

			// Invalidate the object so it can't be used anymore
			INVALIDATE_SLOT(getThis());

			// This is a little hokey. I'm thinking of implementing substatus
			// codes in trogdord responses so I won't have to parse strings
			// like this.
			if (std::string(e.what()).find("slot") != std::string::npos) {
				zend_throw_exception(EXCEPTION_GLOBALS(dumpSlotNotFound), e.what(), e.getCode());
			} else {
				INVALIDATE_DUMP(dump);
				zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), e.getCode());
			}

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

// Destroys the dump slot. Throws an instance of \Trogdord\DumpSlotNotFound if
// the slot no longer exists, an instance of \Trogdord\GameNotFound if the
// dump the slot belongs to no longer exists, and \Trogdord\NetworkException
// if there's an issue with the network connection that prevents this call
// from returning a valid value.
ZEND_BEGIN_ARG_INFO(arginfoDestroy, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Slot, destroy) {

	zval rv; // ???

	zval *dump = SLOT_TO_DUMP(getThis(), &rv);
	zval *trogdord = DUMP_TO_TROGDORD(dump, &rv);
	zval *id = DUMP_TO_ID(dump, &rv);
	zval *slot = SLOT_TO_SLOTVAL(getThis(), &rv);

	ZEND_PARSE_PARAMETERS_NONE();
	ASSERT_DUMP_IS_VALID(DUMP_IS_VALID_PROP(dump, &rv));
	ASSERT_DUMP_SLOT_IS_VALID(SLOT_IS_VALID_PROP(getThis(), &rv));

	try {

		std::string request = SLOT_DESTROY_REQUEST;

		strReplace(request, "%gid", std::to_string(
			IS_LONG == Z_TYPE_P(id) ? Z_LVAL_P(id) : static_cast<int>(Z_DVAL_P(id))
		));
		strReplace(request, "%slot", std::to_string(
			IS_LONG == Z_TYPE_P(slot) ? Z_LVAL_P(slot) : static_cast<int>(Z_DVAL_P(slot))
		));

		trogdordObject *objWrapper = ZOBJ_TO_TROGDORD(Z_OBJ_P(trogdord));

		Document response = Request::execute(
			objWrapper->data.hostname,
			objWrapper->data.port,
			request,
			trogdord
		);

		// Invalidate the object so it can't be used anymore
		INVALIDATE_SLOT(getThis());
	}

	// Throw \Trogord\NetworkException
	catch (const NetworkException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(networkException), e.what(), 0);
		RETURN_NULL();
	}

	catch (const RequestException &e) {

		if (404 == e.getCode()) {

			// Invalidate the object so it can't be used anymore
			INVALIDATE_SLOT(getThis());

			// This is a little hokey. I'm thinking of implementing substatus
			// codes in trogdord responses so I won't have to parse strings
			// like this.
			if (std::string(e.what()).find("slot") != std::string::npos) {
				zend_throw_exception(EXCEPTION_GLOBALS(dumpSlotNotFound), e.what(), e.getCode());
			} else {
				INVALIDATE_DUMP(dump);
				zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), e.getCode());
			}

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

// PHP Slot class methods
static const zend_function_entry classMethods[] =  {
	PHP_ME(Slot, __construct, arginfoCtor, ZEND_ACC_PRIVATE | ZEND_ACC_CTOR)
	PHP_ME(Slot, __get, arginfoMagicGet, ZEND_ACC_PUBLIC)
	PHP_ME(Slot, restore, arginfoRestore, ZEND_ACC_PUBLIC)
	PHP_ME(Slot, destroy, arginfoDestroy, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/*****************************************************************************/

bool createDumpSlotObj(
	zval *slotObj,
	size_t slot,
	size_t timestampMs,
	zval *dumpObj
) {

	if (SUCCESS != object_init_ex(slotObj, SLOT_GLOBALS(classEntry))) {
		return false;
	}

	if (slot > ZEND_LONG_MAX) {
		zend_update_property_double(
			SLOT_GLOBALS(classEntry),
			#if ZEND_MODULE_API_NO >= 20200930 // PHP 8.0+
				Z_OBJ_P(slotObj),
			#else
				slotObj,
			#endif
			SLOT_SLOT_PROPERTY,
			strlen(SLOT_SLOT_PROPERTY),
			slot
		);
	} else {
		zend_update_property_long(
			SLOT_GLOBALS(classEntry),
			#if ZEND_MODULE_API_NO >= 20200930 // PHP 8.0+
				Z_OBJ_P(slotObj),
			#else
				slotObj,
			#endif
			SLOT_SLOT_PROPERTY,
			strlen(SLOT_SLOT_PROPERTY),
			slot
		);
	}

	if (timestampMs > ZEND_LONG_MAX) {
		zend_update_property_double(
			SLOT_GLOBALS(classEntry),
			#if ZEND_MODULE_API_NO >= 20200930 // PHP 8.0+
				Z_OBJ_P(slotObj),
			#else
				slotObj,
			#endif
			SLOT_TIMESTAMP_PROPERTY,
			strlen(SLOT_TIMESTAMP_PROPERTY),
			timestampMs
		);
	} else {
		zend_update_property_long(
			SLOT_GLOBALS(classEntry),
			#if ZEND_MODULE_API_NO >= 20200930 // PHP 8.0+
				Z_OBJ_P(slotObj),
			#else
				slotObj,
			#endif
			SLOT_TIMESTAMP_PROPERTY,
			strlen(SLOT_TIMESTAMP_PROPERTY),
			timestampMs
		);
	}

	zend_update_property(
		SLOT_GLOBALS(classEntry),
		#if ZEND_MODULE_API_NO >= 20200930 // PHP 8.0+
			Z_OBJ_P(slotObj),
		#else
			slotObj,
		#endif
		SLOT_DUMP_PROPERTY,
		strlen(SLOT_DUMP_PROPERTY),
		dumpObj
	);

	zend_update_property_bool(
		SLOT_GLOBALS(classEntry),
		#if ZEND_MODULE_API_NO >= 20200930 // PHP 8.0+
			Z_OBJ_P(slotObj),
		#else
			slotObj,
		#endif
		SLOT_VALID_PROPERTY,
		strlen(SLOT_VALID_PROPERTY),
		1
	);

	return true;
}

/*****************************************************************************/

void defineDumpSlotClass() {

	zend_class_entry dumpSlotClass;

	INIT_CLASS_ENTRY(dumpSlotClass, "Trogdord\\Game\\Dump\\Slot", classMethods);
	SLOT_GLOBALS(classEntry) = zend_register_internal_class(&dumpSlotClass);

	zend_declare_property_null(
		SLOT_GLOBALS(classEntry),
		SLOT_SLOT_PROPERTY,
		strlen(SLOT_SLOT_PROPERTY),
		ZEND_ACC_PRIVATE
	);

	zend_declare_property_null(
		SLOT_GLOBALS(classEntry),
		SLOT_TIMESTAMP_PROPERTY,
		strlen(SLOT_TIMESTAMP_PROPERTY),
		ZEND_ACC_PRIVATE
	);

	zend_declare_property_null(
		SLOT_GLOBALS(classEntry),
		SLOT_DUMP_PROPERTY,
		strlen(SLOT_DUMP_PROPERTY),
		ZEND_ACC_PRIVATE
	);

	zend_declare_property_bool(
		SLOT_GLOBALS(classEntry),
		SLOT_VALID_PROPERTY,
		strlen(SLOT_VALID_PROPERTY),
		0,
		ZEND_ACC_PRIVATE
	);

	// Make sure users can't extend the class
	SLOT_GLOBALS(classEntry)->ce_flags |= ZEND_ACC_FINAL;
}
