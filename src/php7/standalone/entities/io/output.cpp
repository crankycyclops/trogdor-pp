#include "output.h"
#include "gamedata.h"

ZEND_DECLARE_MODULE_GLOBALS(entityout);

zend_object_handlers outputObjectHandlers;

/*****************************************************************************/
/*****************************************************************************/

// Custom Object Handlers
// See: http://blog.jpauli.tech/2016-01-14-php-7-objects-html/

static zend_object *createOutputObject(zend_class_entry *classEntry TSRMLS_DC) {

	outputDataWrapper *wrapper = static_cast<outputDataWrapper *>(ecalloc(
		1, sizeof(*wrapper) + zend_object_properties_size(classEntry)
	));

	wrapper->data.ePtr = nullptr;

	zend_object_std_init(&wrapper->std, classEntry);
	object_properties_init(&wrapper->std, classEntry);

	wrapper->std.handlers = &outputObjectHandlers;

	return &wrapper->std;
}

/*****************************************************************************/

static void destroyOutputObject(zend_object *object TSRMLS_DC) {

	zend_objects_destroy_object(object);
}

/*****************************************************************************/

static void freeOutputObject(zend_object *object TSRMLS_DC) {

	zend_object_std_dtor(object TSRMLS_CC);
}

/*****************************************************************************/
/*****************************************************************************/

// Output Methods

ZEND_BEGIN_ARG_INFO(arginfoOutputCtor, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Output, __construct) {

	php_error_docref(NULL, E_ERROR, "Tried to instantiate non-instantiable class.");
	RETURN_NULL();
}

/*****************************************************************************/

ZEND_BEGIN_ARG_INFO(arginfoOutputConsume, 0)
	ZEND_ARG_INFO(0, channel)
ZEND_END_ARG_INFO()

PHP_METHOD(Output, consume) {

	char *channel;
	size_t channelLength;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &channel, &channelLength) == FAILURE) {
		RETURN_NULL();
	}

	trogdor::entity::Entity *ePtr = ZOBJ_TO_OUTPUTOBJ(Z_OBJ_P(getThis()))->data.ePtr;

	if (!ePtr) {
		php_error_docref(NULL, E_WARNING, "Null Entity pointer encountered in Trogdor\\Entity\\IO\\Output::consume. This is probably a bug and should be reported.");
		RETURN_NULL();
	}

	customConstructedDataMap[ePtr->getGame()]->outBufferMutex.lock();

	if (customConstructedDataMap[ePtr->getGame()]->outBuffer.end() ==
	customConstructedDataMap[ePtr->getGame()]->outBuffer.find(ePtr->getName())) {
		customConstructedDataMap[ePtr->getGame()]->outBufferMutex.unlock();
		RETURN_NULL();
	}

	else if (customConstructedDataMap[ePtr->getGame()]->outBuffer[ePtr->getName()].end() ==
	customConstructedDataMap[ePtr->getGame()]->outBuffer[ePtr->getName()].find(channel)) {
		customConstructedDataMap[ePtr->getGame()]->outBufferMutex.unlock();
		RETURN_NULL();
	}

	else if (customConstructedDataMap[ePtr->getGame()]->outBuffer[ePtr->getName()][channel].empty()) {
		customConstructedDataMap[ePtr->getGame()]->outBufferMutex.unlock();
		RETURN_NULL();
	}

	else {

		OutputMessage &m = customConstructedDataMap[ePtr->getGame()]->outBuffer[ePtr->getName()][channel].front();

		array_init(return_value);
		add_assoc_long(return_value, "timestamp", m.timestamp);
		add_assoc_string(return_value, "message", m.content.c_str());

		customConstructedDataMap[ePtr->getGame()]->outBuffer[ePtr->getName()][channel].pop();
		customConstructedDataMap[ePtr->getGame()]->outBufferMutex.unlock();
	}
}

/*****************************************************************************/

// Trogdor\Entity\IO\Output methods
static const zend_function_entry outputMethods[] =  {
	PHP_ME(Output, __construct, arginfoOutputCtor, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(Output,     consume, arginfoOutputConsume, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/*****************************************************************************/
/*****************************************************************************/

void defineIOOutputClass() {

	zend_class_entry outputClass;

	INIT_CLASS_ENTRY(outputClass, "Trogdor\\Entity\\IO\\Output", outputMethods);
	ENTITYOUT_GLOBALS(classEntry) = zend_register_internal_class(&outputClass);
	ENTITYOUT_GLOBALS(classEntry)->ce_flags |= ZEND_ACC_FINAL;

	// Start out with default object handlers
	memcpy(&outputObjectHandlers, zend_get_std_object_handlers(), sizeof(outputObjectHandlers));

	// Set the specific custom object handlers we need
	ENTITYOUT_GLOBALS(classEntry)->create_object = createOutputObject;
	outputObjectHandlers.free_obj = freeOutputObject;
	outputObjectHandlers.dtor_obj = destroyOutputObject;

	// For an explanation of why this is necessary, see:
	// http://blog.jpauli.tech/2016-01-14-php-7-objects-html/
	outputObjectHandlers.offset = XtOffsetOf(outputDataWrapper, std);
}
