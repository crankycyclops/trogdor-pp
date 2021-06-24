#ifndef TROGDORD_SERVEROUT_H
#define TROGDORD_SERVEROUT_H


#include <queue>

#include <trogdor/game.h>
#include <trogdor/entities/entity.h>
#include <trogdor/iostream/trogout.h>

#include "../../io/message.h"


/*
	Trogdord output stream.

	From C++, an instance of this class must be initialized in 3 steps before
	it can be used. First, it must be instantiated like so:

	std::unique_ptr<ServerOut> outStream = std::make_unique<ServerOut>(gameId);

	// Next, the Entity that uses the output stream must be instantiated
	// (creating an instance of Player for this example):

	player = gamePtr->createPlayer(playerName, outStream, inStream, errStream);

	// Finally, a pointer to the Player object must be passed back to the
	// output stream:

	outStream->setEntity(player);

	If the entity pointer isn't set, an exception will be thrown to warn the
	developer (me) of their nefarious and dastardly ways.
*/
class ServerOut: public trogdor::Trogout {

	private:

		// ID of the game the output stream belongs to
		size_t gameId;

		// The Entity whose output we're processing
		trogdor::entity::Entity *entityPtr = nullptr;

		// If we haven't yet associated the output stream with its entity,
		// we'll buffer output messages here and then push them out as soon as
		// setEntity is called.
		std::queue<output::Message> msgBuffer;

	public:

		// Constructor
		ServerOut() = delete;
		explicit inline ServerOut(size_t gId): gameId(gId) {}

		// Sets the entity the output stream belongs to. If we started
		// receiving output before this was called, we'll remove that output
		// from the queue and push it out now that we know how to route it.
		void setEntity(trogdor::entity::Entity *e);

		// See core/include/iostream/trogout.h for details.
		virtual void flush();

		// See core/include/iostream/trogout.h for details.
		virtual std::unique_ptr<trogdor::Trogout> clone();
};


#endif
