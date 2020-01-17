#ifndef PHP_STREAMOUT_H
#define PHP_STREAMOUT_H


extern "C" {
	#include "php.h"
}

#include <string>
#include <ctime>
#include <memory>

#include <trogdor/game.h>
#include <trogdor/entities/entity.h>
#include <trogdor/iostream/trogout.h>

#include "../gamedata.h"
#include "../exception.h"


/*
	PHP output stream.

	From C++, an instance of this class must be initialized in 3 steps before
	it can be used. First, it must be instantiated like so:

	// gameDataObj is of type customData (see comment for the gameData property
	// for more information.)
	std::unique_ptr<PHPStreamOut> outStream = std::make_unique<PHPStreamOut>(gameDataObj);

	// Next, the Entity that uses the output stream must be instantiated
	// (creating an instance of Player for this example):

	// gamePtr here is a pointer to an instance of trogdor::Game
	gamePtr->createPlayer(playerName, outStream, inStream, errStream);

	// Finally, a pointer to the Player object must be passed back to the
	// output stream:

	outStream->setEntity(gamePtr->getEntity(playerName));

	If the entity pointer isn't set, a PHP exception will be thrown to warn the
	developer (me) of their nefarious and dastardly ways.
*/
class PHPStreamOut: public trogdor::Trogout {

	private:

		// The custom data object associated with the zval containing the PHP
		// instance of Trogdor\Game
		customData *gameData;

		// The Entity the game object belongs to
		trogdor::entity::Entity *entity;

	public:

		// Constructor
		inline PHPStreamOut(customData *d): gameData(d), entity(nullptr) {}

		// Sets the entity the output stream belongs to. This must be
		// called BEFORE the output stream can be used.
		inline void setEntity(trogdor::entity::Entity *e) {entity = e;}

		// See core/include/iostream/trogout.h for details.
		virtual void flush();

		// See core/include/iostream/trogout.h for details.
		virtual std::unique_ptr<trogdor::Trogout> clone();
};


#endif /* PHP_STREAMOUT_H */
