#include "game.h"
#include "module.h"
#include "exception.h"
#include "streamerr.h"

ZEND_DECLARE_MODULE_GLOBALS(game);

zend_object_handlers gameObjectHandlers;

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

// Game methods

PHP_METHOD(Game, __construct) {

	zend_string *XMLPath;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(XMLPath)
	ZEND_PARSE_PARAMETERS_END();

	zval *thisPtr = getThis();
	trogdor::Game *gameObjPtr = ZOBJ_TO_GAMEOBJ(Z_OBJ_P(thisPtr))->realGameObject;

	// Parse the XML game definition
	std::unique_ptr<trogdor::XMLParser> parser = std::make_unique<trogdor::XMLParser>(
		gameObjPtr->makeInstantiator(), gameObjPtr->getVocabulary()
	);

	try {
		gameObjPtr->initialize(parser.get(), ZSTR_VAL(XMLPath));
	}

	catch (trogdor::ValidationException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(validationException), e.what(), 0);
	}

	catch (trogdor::UndefinedException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(undefinedException), e.what(), 0);
	}

	catch (trogdor::ParseException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(parseException), e.what(), 0);
	}

	catch (trogdor::LuaException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(luaException), e.what(), 0);
	}

	catch (trogdor::entity::EntityException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(entityException), e.what(), 0);
	}

	catch (trogdor::entity::BeingException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(beingException), e.what(), 0);
	}

	catch (trogdor::Exception &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(baseException), e.what(), 0);
	}
}

PHP_METHOD(Game, start) {

	zval *thisPtr = getThis();
	trogdor::Game *gameObjPtr = ZOBJ_TO_GAMEOBJ(Z_OBJ_P(thisPtr))->realGameObject;

	try {
		gameObjPtr->start();
	}

	catch (trogdor::Exception &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(baseException), e.what(), 0);
	}

	RETURN_NULL()
}

PHP_METHOD(Game, stop) {

	zval *thisPtr = getThis();
	trogdor::Game *gameObjPtr = ZOBJ_TO_GAMEOBJ(Z_OBJ_P(thisPtr))->realGameObject;

	try {
		gameObjPtr->stop();
	}

	catch (trogdor::Exception &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(baseException), e.what(), 0);
	}

	RETURN_NULL()
}

// PHP Game class methods
static const zend_function_entry gameMethods[] =  {
	PHP_ME(Game, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(Game,       start, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Game,        stop, NULL, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/*****************************************************************************/

void defineGameClass() {

	zend_class_entry gameClass;

	INIT_CLASS_ENTRY(gameClass, "Trogdor\\Game", gameMethods);
	GAME_GLOBALS(gameClassEntry) = zend_register_internal_class(&gameClass);

	// Make sure users can't extend the Game class
	GAME_GLOBALS(gameClassEntry)->ce_flags |= ZEND_ACC_FINAL;

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
