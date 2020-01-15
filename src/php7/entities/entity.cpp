#include "entity.h"

ZEND_DECLARE_MODULE_GLOBALS(entity);

zend_object_handlers entityObjectHandlers;

/*****************************************************************************/
/*****************************************************************************/

// Custom Object Handlers
// See: http://blog.jpauli.tech/2016-01-14-php-7-objects-html/

static zend_object *createEntityObject(zend_class_entry *classEntry TSRMLS_DC) {

	entityObject *gObj = (entityObject *)ecalloc(1, sizeof(*gObj) + zend_object_properties_size(classEntry));

	// If managedByGame is true, it means the object belongs to the game and
	// should not be freed at any time by PHP
	gObj->realEntityObject.managedByGame = false;
	gObj->realEntityObject.obj = nullptr;

	zend_object_std_init(&gObj->std, classEntry);
	object_properties_init(&gObj->std, classEntry);

	gObj->std.handlers = &entityObjectHandlers;

	return &gObj->std;
}

/*****************************************************************************/

static void destroyEntityObject(zend_object *object TSRMLS_DC) {

	zend_objects_destroy_object(object);
}

/*****************************************************************************/

static void freeEntityObject(zend_object *object TSRMLS_DC) {

	// If the Entity isn't being managed by the game, we need to free it
	if (!ZOBJ_TO_ENTITYOBJ(object)->realEntityObject.managedByGame) {
		delete ZOBJ_TO_ENTITYOBJ(object)->realEntityObject.obj;
	}

	zend_object_std_dtor(object TSRMLS_CC);
}

/*****************************************************************************/
/*****************************************************************************/

// Entity Methods

// PHP Entity class methods
static const zend_function_entry entityMethods[] =  {
	PHP_FE_END
};

/*****************************************************************************/
/*****************************************************************************/

// TODO: this should be an abstract class
void defineEntityClass() {

	zend_class_entry entityClass;

	INIT_CLASS_ENTRY(entityClass, "Trogdor\\Entity\\Entity", entityMethods);
	ENTITY_GLOBALS(classEntry) = zend_register_internal_class(&entityClass);
	ENTITY_GLOBALS(classEntry)->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;

	// Declare the Entity class's properties
	zend_declare_property_null(ENTITY_GLOBALS(classEntry), "name", sizeof("name") - 1, ZEND_ACC_PROTECTED TSRMLS_CC);

	// Start out with default object handlers
	memcpy(&entityObjectHandlers, zend_get_std_object_handlers(), sizeof(entityObjectHandlers));

	// Set the specific custom object handlers we need
	ENTITY_GLOBALS(classEntry)->create_object = createEntityObject;
	entityObjectHandlers.free_obj = freeEntityObject;
	entityObjectHandlers.dtor_obj = destroyEntityObject;

	// For an explanation of why this is necessary, see:
	// http://blog.jpauli.tech/2016-01-14-php-7-objects-html/
	entityObjectHandlers.offset = XtOffsetOf(entityObject, std);
}
