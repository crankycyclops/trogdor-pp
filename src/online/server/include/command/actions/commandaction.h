#ifndef COMMANDACTION_H
#define COMMANDACTION_H


#include "../networkaction.h"


/*
	Receives a player's command from the client and passes it to the game to be
	processed.

	Workflow:

	1) execute() checks if the connection is associated with a player. If not,
	   then we return AUTHREQ. If one is, we spin off a separate thread that
	   runs processCommandThread. We need to run this inside a separate thread
	   because TCPIn will block while it waits to read data from the client.

	2) inside processCommandThread, we retrieve the player, bind the TCPIn
	   object to our current connection and run game->processCommand(player.)

	3) game->processCommand(player) will read one or more lines of input from
	   the user's client. To do this, we write to the client the command GET,
	   instructing the client to send a raw string representing the command
	   (or subsequently requested data.)

	4) processCommand ultimately calls TCPIn's input operator, which blocks
	   the thread on a mutex so that only one operation can be performed on
	   any given TCPIn object at a time.

	5) TCPIn writes out GET and waits for the client's response. It then sleeps
	   (instead of blocking, so that we can continue to asynchronously accept
	   requests in the background) until we get back the client's response,
	   afterwhich we output the string from the stream operator and unlock
	   the mutex.

	6) The thread ends and we mark the connection as free for more requests.
*/
class COMMANDAction: public NetworkAction {

	private:

		// Thread that processes a player command.
		static void *processCommandThread(void *connection);

		virtual void execute(TCPConnection::ptr connection);
};


#endif

