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

		// True when a blocking read operation is in progress.
		bool blocked = false;

		// Set this to true to cancel a read operation that's in progress
		// (will cause the input value to be returned as an empty string.)
		bool killSwitch = false;

		// How many milliseconds to sleep before polling for input
		static const unsigned int INPUT_POLL_INTERVAL = 10;

		// ID of the game the input stream belongs to
		size_t gameId;

		// The Entity who's input we're reading
		trogdor::entity::Entity *entityPtr = nullptr;

	public:

		// Constructor
		ServerIn() = delete;
		explicit inline ServerIn(size_t gId): gameId(gId) {}

		// Returns true if a blocking read operation is in progress and false
		// if not.
		inline bool isBlocked() {return blocked;}

		// Call this if you need to cancel a read operation that's currently
		// in progress. The effect will be that an empty string is read as the
		// input. Note: in order to prevent a possible race condition where
		// kill() was called as part of _unsubscribe before the input stream
		// was accessed by the instance of PlayerFuture to read a command, I
		// removed the check for isBlocked(). If any bugs are encountered
		// because of this, I'll have to revisit this method.
		inline void kill() {

			killSwitch = true;
		}

		// Sets the entity the input stream belongs to. This must be
		// called BEFORE the stream can be used.
		inline void setEntity(trogdor::entity::Entity *e) {entityPtr = e;}

		// See include/iostream/trogin.h for details.
		virtual std::unique_ptr<Trogin> clone();

		// For now, I only need to define input for strings
		virtual Trogin &operator>> (std::string &val);
};


#endif
