#include "../trogdord.h"
#include "../game.h"
#include "../utility.h"
#include "../request.h"

#include "../phpexception.h"
#include "../exception/requestexception.h"

#include "player.h"

ZEND_DECLARE_MODULE_GLOBALS(player);
ZEND_EXTERN_MODULE_GLOBALS(game);

// Exception message when methods are called on a game that's already been destroyed
const char *PLAYER_ALREADY_DESTROYED = "Player has already been destroyed";

// This request removes a player from a game
static const char *PLAYER_DESTROY_REQUEST = "{\"method\":\"delete\",\"scope\":\"player\",\"args\":{\"game_id\":%gid,\"name\":\"%pname\"%msgarg}}";

/*****************************************************************************/

// Removes the player from the game. Throws an instance of
// \Trogdord\NetworkException if the call fails due to network connectivity
// issues. If the game the player belongs to no longer exists,
// \TrogdordGameNotFound will be thrown. If the player has already been
// destroyed, \Trogdord\PlayerNotFound will be thrown.
ZEND_BEGIN_ARG_INFO(arginfoDestroy, 0)
	ZEND_ARG_TYPE_INFO(0, removeMsg, IS_STRING, 1)
ZEND_END_ARG_INFO()

PHP_METHOD(Player, destroy) {

	zval rv; // ???

	zval *pName = ENTITY_TO_NAME(getThis(), &rv);
	zval *game = ENTITY_TO_GAME(getThis(), &rv);
	zval *trogdord = GAME_TO_TROGDORD(game, &rv);
	zval *gameId = GAME_TO_ID(game, &rv);

	char *removeMsg = nullptr;
	size_t removeMsgLen = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &removeMsg, &removeMsgLen) == FAILURE) {
		RETURN_NULL();
	}

	ASSERT_GAME_ID_IS_VALID(Z_TYPE_P(gameId));
	ASSERT_PLAYER_NAME_IS_VALID(Z_TYPE_P(pName));

	try {

		std::string request = PLAYER_DESTROY_REQUEST;
		strReplace(request, "%gid", std::to_string(Z_LVAL_P(gameId)));
		strReplace(request, "%pname", Z_STRVAL_P(pName));
		strReplace(request, "%msgarg", removeMsgLen ? std::string(",\"message\":\"") + removeMsg + "\"" : "");

		trogdordObject *trogdordObjWrapper = ZOBJ_TO_TROGDORD(Z_OBJ_P(trogdord));

		JSONObject response = Request::execute(
			trogdordObjWrapper->data.hostname,
			trogdordObjWrapper->data.port,
			request
		);

		// Set the player name to null so the object can't be used anymore
		zend_update_property_null(
			ENTITY_GLOBALS(classEntry),
			getThis(),
			NAME_PROPERTY_NAME,
			strlen(NAME_PROPERTY_NAME)
		);
	}

	// Throw \Trogord\NetworkException
	catch (const NetworkException &e) {
		zend_throw_exception(EXCEPTION_GLOBALS(networkException), e.what(), 0);
		RETURN_NULL();
	}

	catch (const RequestException &e) {

		if (404 == e.getCode()) {

			// Throw \Trogdord\GameNotFound
			if (0 == strcmp(e.what(), "game not found")) {
				zend_throw_exception(EXCEPTION_GLOBALS(gameNotFound), e.what(), e.getCode());
				RETURN_NULL();
			}

			// Throw \Trogdord\PlayerNotFound
			else {
				zend_throw_exception(EXCEPTION_GLOBALS(playerNotFound), e.what(), e.getCode());
				RETURN_NULL();
			}
		}

		// Throw \Trogdord\RequestException
		else {
			zend_throw_exception(EXCEPTION_GLOBALS(requestException), e.what(), e.getCode());
			RETURN_NULL();
		}
	}
}

/*****************************************************************************/

// PHP Player class methods
static const zend_function_entry playerMethods[] =  {
	PHP_ME(Player, destroy, arginfoDestroy, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/*****************************************************************************/

void definePlayerClass() {

	zend_class_entry playerClass;

	INIT_CLASS_ENTRY(playerClass, "Trogdord\\Player", playerMethods);
	PLAYER_GLOBALS(classEntry) = zend_register_internal_class_ex(&playerClass, BEING_GLOBALS(classEntry));

	// Player is a concrete entity type and can't be extended further
	PLAYER_GLOBALS(classEntry)->ce_flags |= ZEND_ACC_FINAL;
}