#include <cstring>

#include "entity.h"
#include "io/input.h"
#include "io/output.h"

ZEND_DECLARE_MODULE_GLOBALS(entity);

zend_object_handlers entityObjectHandlers;

/*****************************************************************************/
/*****************************************************************************/

// Custom Object Handlers
// See: http://blog.jpauli.tech/2016-01-14-php-7-objects-html/

static zend_object *createEntityObject(zend_class_entry *classEntry TSRMLS_DC) {

	entityObject *gObj = (entityObject *)ecalloc(
		1, sizeof(*gObj) + zend_object_properties_size(classEntry)
	);

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

WRITE_PROP_RETURN_TYPE writeProperty(zval *object, zval *member, zval *value, void **cache_slot) {

	if (IS_STRING == Z_TYPE_P(member) && (
		0 == strcmp("input", (Z_STRVAL_P(member))) ||
		0 == strcmp("output", Z_STRVAL_P(member)))
	) {
		std::string warning = std::string("Cannot write to read-only property '")
			+ Z_STRVAL_P(member) + "'";
		php_error_docref(NULL, E_WARNING, warning.c_str());
	} else {
		#if ZEND_MODULE_API_NO >= 20190902
			return zend_std_write_property(object, member, value, cache_slot);
		#else
			zend_std_write_property(object, member, value, cache_slot);
		#endif
	}

	#if ZEND_MODULE_API_NO >= 20190902
		return object;
	#endif
}

/*****************************************************************************/
/*****************************************************************************/

// Magic "__get" allows us to make private and protected data members read-only
ZEND_BEGIN_ARG_INFO(arginfoEntity__Get, 0)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

PHP_METHOD(Entity, __get) {

	char *name;
	int nameLength;

	zval *propVal, rv;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &nameLength) == FAILURE) {
		RETURN_NULL()
	}

	// If I return whatever zend_read_property sets rv to, I get an UNKNOWN
	// zval type when I var_dump the result, but when I return
	// zend_read_property's return value instead, it works like it's supposed
	// to. WTF is the rv argument for? Dunno, because PHP's documentation
	// regarding internals is non-existent. Oh well.
	propVal = zend_read_property(ENTITY_GLOBALS(classEntry), getThis(), name, nameLength, 1, &rv TSRMLS_CC);

	// There's some insanity in how this works, so for reference, here's what
	// I read to help me understand what all the arguments mean:
	// https://medium.com/@davidtstrauss/copy-and-move-semantics-of-zvals-in-php-7-41427223d784
	RETURN_ZVAL(propVal, 1, 0);
}

/*****************************************************************************/

// Entity Methods

// PHP Entity class methods
static const zend_function_entry entityMethods[] =  {
	PHP_ME(Entity, __get, arginfoEntity__Get, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/*****************************************************************************/
/*****************************************************************************/

// Utility functions

void attachInputToEntity(trogdor::entity::Entity *ePtr, zval *phpEntityObj) {

	zval inputObj;

	if (SUCCESS != object_init_ex(&inputObj, ENTITYIN_GLOBALS(classEntry))) {
		php_error_docref(NULL, E_ERROR, "Could not instantiate Trogdor\\Entity\\IO\\Input. This is a bug.");
	}

	// The input buffer needs a pointer to the Entity it's assigned to so
	// it can key into the output buffer.
	ZOBJ_TO_OUTPUTOBJ(Z_OBJ(inputObj))->data.ePtr = ePtr;

	// Temporarily make input writeable.
	entityObjectHandlers.write_property = zend_std_write_property;

	// This read-only property is the object we use to consume messages
	// from the output buffer.
	zend_update_property(
		ENTITY_GLOBALS(classEntry),
		phpEntityObj,
		"input",
		sizeof("input") - 1,
		&inputObj TSRMLS_DC
	);

	// Once we've updated the output property, make it read-only again so
	// it can't be modified from PHP userland.
	entityObjectHandlers.write_property = writeProperty;

	zval_ptr_dtor(&inputObj);
}

/*****************************************************************************/

void attachOutputToEntity(trogdor::entity::Entity *ePtr, zval *phpEntityObj) {

	zval outputObj;

	if (SUCCESS != object_init_ex(&outputObj, ENTITYOUT_GLOBALS(classEntry))) {
		php_error_docref(NULL, E_ERROR, "Could not instantiate Trogdor\\Entity\\IO\\Output. This is a bug.");
	}

	// The output buffer needs a pointer to the Entity it's assigned to so
	// it can key into the output buffer.
	ZOBJ_TO_OUTPUTOBJ(Z_OBJ(outputObj))->data.ePtr = ePtr;

	// Temporarily make output writeable.
	entityObjectHandlers.write_property = zend_std_write_property;

	// This read-only property is the object we use to consume messages
	// from the output buffer.
	zend_update_property(
		ENTITY_GLOBALS(classEntry),
		phpEntityObj,
		"output",
		sizeof("output") - 1,
		&outputObj TSRMLS_DC
	);

	// Once we've updated the output property, make it read-only again so
	// it can't be modified from PHP userland.
	entityObjectHandlers.write_property = writeProperty;

	zval_ptr_dtor(&outputObj);
}

/*****************************************************************************/

void refreshEntityProperties(trogdor::entity::Entity *ePtr, zval *phpEntityObj) {

	zend_update_property_string(
		ENTITY_GLOBALS(classEntry),
		phpEntityObj,
		"name",
		sizeof("name") - 1,
		ePtr->getName().c_str() TSRMLS_DC
	);

	zend_update_property_string(
		ENTITY_GLOBALS(classEntry),
		phpEntityObj,
		"title",
		sizeof("title") - 1,
		ePtr->getTitle().c_str() TSRMLS_DC
	);

	zend_update_property_string(
		ENTITY_GLOBALS(classEntry),
		phpEntityObj,
		"longDesc",
		sizeof("longDesc") - 1,
		ePtr->getLongDescription().c_str() TSRMLS_DC
	);

	zend_update_property_string(
		ENTITY_GLOBALS(classEntry),
		phpEntityObj,
		"shortDesc",
		sizeof("shortDesc") - 1,
		ePtr->getShortDescription().c_str() TSRMLS_DC
	);
}

/*****************************************************************************/

void defineEntityClass() {

	zval *ioout;

	defineIOInputClass();
	defineIOOutputClass();

	zend_class_entry entityClass;

	INIT_CLASS_ENTRY(entityClass, "Trogdor\\Entity\\Entity", entityMethods);
	ENTITY_GLOBALS(classEntry) = zend_register_internal_class(&entityClass);
	ENTITY_GLOBALS(classEntry)->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;

	// Declare the Entity class's properties
	zend_declare_property_null(
		ENTITY_GLOBALS(classEntry),
		"input",
		sizeof("input") - 1,
		ZEND_ACC_PUBLIC TSRMLS_CC
	);

	zend_declare_property_null(
		ENTITY_GLOBALS(classEntry),
		"output",
		sizeof("output") - 1,
		ZEND_ACC_PUBLIC TSRMLS_CC
	);

	zend_declare_property_null(
		ENTITY_GLOBALS(classEntry),
		"name",
		sizeof("name") - 1,
		ZEND_ACC_PROTECTED TSRMLS_CC
	);

	zend_declare_property_null(
		ENTITY_GLOBALS(classEntry),
		"title",
		sizeof("title") - 1,
		ZEND_ACC_PROTECTED TSRMLS_CC
	);

	zend_declare_property_null(
		ENTITY_GLOBALS(classEntry),
		"longDesc",
		sizeof("longDesc") - 1,
		ZEND_ACC_PROTECTED TSRMLS_CC
	);

	zend_declare_property_null(
		ENTITY_GLOBALS(classEntry),
		"shortDesc",
		sizeof("shortDesc") - 1,
		ZEND_ACC_PROTECTED TSRMLS_CC
	);

	// Start out with default object handlers
	memcpy(&entityObjectHandlers, zend_get_std_object_handlers(), sizeof(entityObjectHandlers));

	// Set the specific custom object handlers we need
	ENTITY_GLOBALS(classEntry)->create_object = createEntityObject;
	entityObjectHandlers.free_obj = freeEntityObject;
	entityObjectHandlers.dtor_obj = destroyEntityObject;
	entityObjectHandlers.write_property = writeProperty;

	// For an explanation of why this is necessary, see:
	// http://blog.jpauli.tech/2016-01-14-php-7-objects-html/
	entityObjectHandlers.offset = XtOffsetOf(entityObject, std);
}