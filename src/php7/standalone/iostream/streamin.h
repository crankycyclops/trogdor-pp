#ifndef PHP_STREAMIN_H
#define PHP_STREAMIN_H


extern "C" {
	#include "php.h"
}

#include <string>
#include <memory>

#include <trogdor/game.h>
#include <trogdor/entities/entity.h>
#include <trogdor/iostream/trogout.h>

#include "../gamedata.h"
#include "../exception.h"


/*
	PHP input stream.

	From C++, an instance of this class must be initialized in 3 steps before
	it can be used. First, it must be instantiated like so:

	// gameDataObj is of type customData (see comment for the gameData property
	// for more information.)
	std::unique_ptr<PHPStreamIn> inStream = std::make_unique<PHPStreamIn>(gameDataObj);

	// Next, the Entity that uses the input stream must be instantiated
	// (creating an instance of Player for this example):

	// gamePtr here is a pointer to an instance of trogdor::Game
	player = gamePtr->createPlayer(playerName, outStream, inStream, errStream);

	// Finally, a pointer to the Player object must be passed back to the
	// input stream:

	inStream->setEntity(player);
*/
class PHPStreamIn: public trogdor::Trogin {

	private:

		// The custom data object associated with the zval containing the PHP
		// instance of Trogdor\Game
		customData *gameData;

		// The Entity the game object belongs to
		trogdor::entity::Entity *entity;

	public:

		// Constructor
		explicit inline PHPStreamIn(customData *d): gameData(d), entity(nullptr) {}

		// Sets the entity the output stream belongs to. This must be
		// called BEFORE the output stream can be used.
		inline void setEntity(trogdor::entity::Entity *e) {entity = e;}

		// See include/iostream/trogin.h for details.
		virtual std::unique_ptr<trogdor::Trogin> clone();

		// See include/iostream/trogin.h for details.
		virtual trogdor::Trogin &operator>> (std::string &val);
};


#endif /* PHP_STREAMIN_H */
