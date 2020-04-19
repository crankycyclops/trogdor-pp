#include "exception.h"

ZEND_DECLARE_MODULE_GLOBALS(exception);

/*****************************************************************************/

void defineExceptionClasses() {

	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "Trogdor\\Exception", NULL);
	EXCEPTION_GLOBALS(baseException) = zend_register_internal_class_ex(&ce, zend_ce_exception);

	INIT_CLASS_ENTRY(ce, "Trogdor\\ValidationException", NULL);
	EXCEPTION_GLOBALS(validationException) = zend_register_internal_class_ex(&ce, EXCEPTION_GLOBALS(baseException));

	INIT_CLASS_ENTRY(ce, "Trogdor\\UndefinedException", NULL);
	EXCEPTION_GLOBALS(undefinedException) = zend_register_internal_class_ex(&ce, EXCEPTION_GLOBALS(baseException));

	INIT_CLASS_ENTRY(ce, "Trogdor\\LuaException", NULL);
	EXCEPTION_GLOBALS(luaException) = zend_register_internal_class_ex(&ce, EXCEPTION_GLOBALS(baseException));

	INIT_CLASS_ENTRY(ce, "Trogdor\\EntityException", NULL);
	EXCEPTION_GLOBALS(entityException) = zend_register_internal_class_ex(&ce, EXCEPTION_GLOBALS(baseException));

	INIT_CLASS_ENTRY(ce, "Trogdor\\BeingException", NULL);
	EXCEPTION_GLOBALS(beingException) = zend_register_internal_class_ex(&ce, EXCEPTION_GLOBALS(entityException));

	INIT_CLASS_ENTRY(ce, "Trogdor\\ParseException", NULL);
	EXCEPTION_GLOBALS(parseException) = zend_register_internal_class_ex(&ce, EXCEPTION_GLOBALS(baseException));
}
