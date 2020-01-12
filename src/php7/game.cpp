#include "game.h"
#include "streamerr.h"

ZEND_DECLARE_MODULE_GLOBALS(game);

zend_object_handlers gameObjectHandlers;

// PHP Game class methods
static const zend_function_entry gameMethods[] =  {
	// TODO: define methods
	PHP_FE_END
};

/*****************************************************************************/

// Custom Object Handlers
// See: http://blog.jpauli.tech/2016-01-14-php-7-objects-html/

static zend_object *createGameObject(zend_class_entry *classEntry TSRMLS_DC) {

	gameObject *gObj;

	gObj = (gameObject *)ecalloc(1, sizeof(*gObj) + zend_object_properties_size(classEntry));
	gObj->realGameObject = new trogdor::Game(
		std::make_unique<PHPStreamErr>()
	);

	zend_object_std_init(&gObj->std, classEntry);
	object_properties_init(&gObj->std, classEntry);

	gObj->std.handlers = &gameObjectHandlers;

	return &gObj->std;
}

static void destroyGameObject(zend_object *object TSRMLS_DC) {

	// If the game is running, be polite and stop it before it's freed
	ZOBJ_TO_GAMEOBJ(object)->realGameObject->stop();
	zend_objects_destroy_object(object);
}

static void freeGameObject(zend_object *object TSRMLS_DC) {

	delete ZOBJ_TO_GAMEOBJ(object)->realGameObject;
	zend_object_std_dtor(object TSRMLS_CC);
}

/*****************************************************************************/

void defineGameClass() {

	zend_class_entry gameClass;

	INIT_CLASS_ENTRY(gameClass, "Trogdor\\Game", gameMethods);
	// gameClass.create_object = gameObjectNew; (TODO: DO I NEED THIS?)
	GAME_GLOBALS(gameClassEntry) = zend_register_internal_class(&gameClass);

	// Start out with default object handlers
	memcpy(&gameObjectHandlers, zend_get_std_object_handlers(), sizeof(gameObjectHandlers));

	// Set the specific custom object handlers we need
	GAME_GLOBALS(gameClassEntry)->create_object = createGameObject;
	gameObjectHandlers.free_obj = freeGameObject;
	gameObjectHandlers.dtor_obj = destroyGameObject;

	// For an explanation of why this is necessary, see:
	// http://blog.jpauli.tech/2016-01-14-php-7-objects-html/
	gameObjectHandlers.offset = XtOffsetOf(gameObject, std);
}
