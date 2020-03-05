#include "trogdord.h"
#include "exception.h"

ZEND_DECLARE_MODULE_GLOBALS(trogdord);

/*****************************************************************************/
/*****************************************************************************/

// Trogdord Constructor (returned instance represents a connection to an
// instance of trogdord over the network.) Throws instance of
// \Trogdord\NetworkException if the attempt to connect is unsuccessful.
ZEND_BEGIN_ARG_INFO(arginfoCtor, 0)
	ZEND_ARG_TYPE_INFO(0, hostname, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, port, IS_LONG, 1)
ZEND_END_ARG_INFO()

PHP_METHOD(Trogdord, __construct) {

	char *hostname;
	size_t hostnameLength;
	long port = TROGDORD_DEFAULT_PORT;

	zend_parse_parameters_throw(
		ZEND_NUM_ARGS() TSRMLS_CC,
		"s|l",
		&hostname,
		&hostnameLength,
		&port
	);

	// TODO
}

/*****************************************************************************/

// Returns various server-specific statistics from an instance of trogdord.
// Throws an instance of \Trogdord\NetworkException if there's an issue with the
// network connection that prevents this call from returning valid data.
ZEND_BEGIN_ARG_INFO(arginfoStatistics, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Trogdord, statistics) {

	// Method takes no arguments
	ZEND_PARSE_PARAMETERS_NONE();

	// TODO
}

/*****************************************************************************/

// Returns a list of all existing games in an instance of trogdord (could be an
// empty array if no games currently exist.) Throws an instance of
// \Trogdord\NetworkException if there's an issue with the network connection
// that prevents this call from returning a valid list.
ZEND_BEGIN_ARG_INFO(arginfoListGames, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Trogdord, listGames) {

	// Method takes no arguments
	ZEND_PARSE_PARAMETERS_NONE();

	// TODO
}

/*****************************************************************************/

// Returns a list of all game definitions available to an instance of trogdord
// (could be an empty array if no definitions are available.) Throws an instance
// of \Trogdord\NetworkException if there's an issue with the network connection
// that prevents this call from returning a valid list.
ZEND_BEGIN_ARG_INFO(arginfoListDefinitions, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Trogdord, listDefinitions) {

	// Method takes no arguments
	ZEND_PARSE_PARAMETERS_NONE();

	// TODO
}

/*****************************************************************************/

// Returns an instance of \Trogdord\Game, which provides an interface to a game
// that exists inside an instance of trogdord. Throws an instance of
// \Trogdord\GameNotFound if the game doesn't exist and
// \Trogdord\NetworkException if there's an issue with the network connection
// that prevents this call from returning a valid value.
ZEND_BEGIN_ARG_INFO(arginfoGetGame, 0)
	ZEND_ARG_TYPE_INFO(0, id, IS_LONG, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Trogdord, getGame) {

	long gameId;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &gameId)  == FAILURE) {
		RETURN_NULL();
	}

	// TODO
}

/*****************************************************************************/

// Creates a new game inside an instance of trogdord and returns an instance of
// the PHP class \Trogdord\Game, which wraps around it. Throws
// \Trogdord\RequestException if there's an issue creating the game and
// \Trogdord\NetworkException if there's an issue with the network connection
// that prevents this call from creating a new game.
ZEND_BEGIN_ARG_INFO(arginfoNewGame, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, definition, IS_STRING, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Trogdord, newGame) {

	char *name;
	size_t nameLength;

	char *definition;
	size_t definitionLength;

	if (zend_parse_parameters(
		ZEND_NUM_ARGS() TSRMLS_CC,
		"ss",
		&name,
		&nameLength,
		&definition,
		&definitionLength
	)  == FAILURE) {
		RETURN_NULL();
	}

	// TODO
}

/*****************************************************************************/

// PHP Trogdord class methods
static const zend_function_entry classMethods[] =  {
	PHP_ME(Trogdord, __construct, arginfoCtor, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(Trogdord, statistics, arginfoStatistics, ZEND_ACC_PUBLIC)
	PHP_ME(Trogdord, listGames, arginfoListGames, ZEND_ACC_PUBLIC)
	PHP_ME(Trogdord, listDefinitions, arginfoListDefinitions, ZEND_ACC_PUBLIC)
	PHP_ME(Trogdord, getGame, arginfoGetGame, ZEND_ACC_PUBLIC)
	PHP_ME(Trogdord, newGame, arginfoNewGame, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/*****************************************************************************/

void defineTrogdordClass() {

	zend_class_entry trogdordClass;

	INIT_CLASS_ENTRY(trogdordClass, "Trogdord", classMethods);
	TROGDORD_GLOBALS(classEntry) = zend_register_internal_class(&trogdordClass);

	// Make sure users can't extend the class
	TROGDORD_GLOBALS(classEntry)->ce_flags |= ZEND_ACC_FINAL;
}
