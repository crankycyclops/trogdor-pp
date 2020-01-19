#include "input.h"
#include "gamedata.h"

ZEND_DECLARE_MODULE_GLOBALS(entityin);

zend_object_handlers inputObjectHandlers;

/*****************************************************************************/
/*****************************************************************************/

// Custom Object Handlers
// See: http://blog.jpauli.tech/2016-01-14-php-7-objects-html/

static zend_object *createInputObject(zend_class_entry *classEntry TSRMLS_DC) {

	inputDataWrapper *wrapper = (inputDataWrapper *)ecalloc(
		1, sizeof(*wrapper) + zend_object_properties_size(classEntry)
	);

	wrapper->data.ePtr = nullptr;

	zend_object_std_init(&wrapper->std, classEntry);
	object_properties_init(&wrapper->std, classEntry);

	wrapper->std.handlers = &inputObjectHandlers;

	return &wrapper->std;
}

/*****************************************************************************/

static void destroyInputObject(zend_object *object TSRMLS_DC) {

	zend_objects_destroy_object(object);
}

/*****************************************************************************/

static void freeInputObject(zend_object *object TSRMLS_DC) {

	zend_object_std_dtor(object TSRMLS_CC);
}

/*****************************************************************************/
/*****************************************************************************/

// Input Methods

ZEND_BEGIN_ARG_INFO(arginfoInputCtor, 0)
	ZEND_ARG_INFO(0, XMLPath)
ZEND_END_ARG_INFO()

PHP_METHOD(Input, __construct) {

	php_error_docref(NULL, E_ERROR, "Tried to instantiate non-instantiable class.");
	RETURN_NULL();
}

/*****************************************************************************/

ZEND_BEGIN_ARG_INFO(arginfoInputSend, 0)
	ZEND_ARG_INFO(0, inputStr)
ZEND_END_ARG_INFO()

PHP_METHOD(Input, send) {

	char *inputStr;
	size_t inputStrLength;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &inputStr, &inputStrLength) == FAILURE) {
		RETURN_NULL();
	}

	trogdor::entity::Entity *ePtr = ZOBJ_TO_INPUTOBJ(Z_OBJ_P(getThis()))->data.ePtr;

	if (!ePtr) {
		php_error_docref(NULL, E_WARNING, "Null Entity pointer encountered in Trogdor\\Entity\\IO\\Input::send. This is probably a bug and should be reported.");
		RETURN_NULL();
	}

	customConstructedDataMap[ePtr->getGame()]->inBufferMutex.lock();

	if (customConstructedDataMap[ePtr->getGame()]->inBuffer.end() ==
	customConstructedDataMap[ePtr->getGame()]->inBuffer.find(ePtr->getName())) {
		customConstructedDataMap[ePtr->getGame()]->inBuffer[ePtr->getName()] = {};
	}

	customConstructedDataMap[ePtr->getGame()]->inBuffer[ePtr->getName()].push(inputStr);
	customConstructedDataMap[ePtr->getGame()]->inBufferMutex.unlock();
}

/*****************************************************************************/

// Trogdor\Entity\IO\Input methods
static const zend_function_entry inputMethods[] =  {
	PHP_ME(Input, __construct, arginfoInputCtor, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(Input,        send, arginfoInputSend, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/*****************************************************************************/
/*****************************************************************************/

void defineIOInputClass() {

	zend_class_entry inputClass;

	INIT_CLASS_ENTRY(inputClass, "Trogdor\\Entity\\IO\\Input", inputMethods);
	ENTITYIN_GLOBALS(classEntry) = zend_register_internal_class(&inputClass);
	ENTITYIN_GLOBALS(classEntry)->ce_flags |= ZEND_ACC_FINAL;

	// Start out with default object handlers
	memcpy(&inputObjectHandlers, zend_get_std_object_handlers(), sizeof(inputObjectHandlers));

	// Set the specific custom object handlers we need
	ENTITYIN_GLOBALS(classEntry)->create_object = createInputObject;
	inputObjectHandlers.free_obj = freeInputObject;
	inputObjectHandlers.dtor_obj = destroyInputObject;

	// For an explanation of why this is necessary, see:
	// http://blog.jpauli.tech/2016-01-14-php-7-objects-html/
	inputObjectHandlers.offset = XtOffsetOf(inputDataWrapper, std);
}
