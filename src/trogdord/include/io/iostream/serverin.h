#ifndef TROGDORD_SERVERIN_H
#define TROGDORD_SERVERIN_H


#include <trogdor/game.h>
#include <trogdor/entities/entity.h>
#include <trogdor/iostream/trogout.h>


/*
	Trogdord input stream.

	From C++, an instance of this class must be initialized in 3 steps before
	it can be used. First, it must be instantiated like so:

	std::unique_ptr<ServerIn> inStream = std::make_unique<ServerIn>(gameId);

	// Next, the Entity that uses the output stream must be instantiated
	// (creating an instance of Player for this example):

	player = gamePtr->createPlayer(playerName, outStream, inStream, errStream);

	// Finally, a pointer to the Player object must be passed back to the
	// output stream:

	inStream->setEntity(player);

	If the entity pointer isn't set, an exception will be thrown to warn the
	developer (me) of their nefarious and dastardly ways.
*/
class ServerIn: public trogdor::Trogin {

	private:

		// ID of the game the input stream belongs to
		size_t gameId;

		// The Entity who's input we're reading
		trogdor::entity::Entity *entityPtr;

	public:

		// Constructor
		ServerIn() = delete;
		inline ServerIn(size_t gId): gameId(gId) {}

		// Sets the entity the input stream belongs to. This must be
		// called BEFORE the stream can be used.
		inline void setEntity(trogdor::entity::Entity *e) {entityPtr = e;}

		// See include/iostream/trogin.h for details.
		virtual std::unique_ptr<Trogin> clone();

		// For now, I only need to define input for strings
		virtual Trogin &operator>> (std::string &val);
};


#endif
