#ifndef COMMANDACTION_H
#define COMMANDACTION_H


#include "../networkaction.h"

#include "../../../../../core/include/thread.h"


// Receives a player's command from the client and passes it to the game to be
// processed.
class COMMANDAction: public NetworkAction {

	private:

		thread_t commandThread;

		// Thread that processes a player command.
		static void *processCommandThread(void *connection);

		virtual void execute(TCPConnection::ptr connection);
};


#endif

