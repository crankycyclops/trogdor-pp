#include <Zend/zend_long.h>

#include "../trogdord.h"
#include "../json.h"
#include "../request.h"

#include "../phpexception.h"
#include "../exception/requestexception.h"

#include "dump.h"

ZEND_DECLARE_MODULE_GLOBALS(dump);
ZEND_EXTERN_MODULE_GLOBALS(trogdord);

/*****************************************************************************/

// PHP Dump class methods
static const zend_function_entry classMethods[] =  {
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
