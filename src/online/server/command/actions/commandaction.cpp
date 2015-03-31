#include <string>
#include <boost/utility/addressof.hpp>

#include "../../../../core/include/game.h"
#include "../../../../core/include/entities/player.h"

#include "../../include/command/actions/commandaction.h"
#include "../../include/iostream/tcpin.h"
#include "../../include/iostream/tcpout.h"

#include "../../include/main.h"


void *COMMANDAction::processCommandThread(void *connection) {

	TCPConnection::ptr *c = ((TCPConnection::ptr *)connection);
	vector<string> request = (*c)->getBufferParts();

	core::entity::Player *player = currentGame->getPlayer(request[1]);

	dynamic_cast<TCPIn &>(player->in()).setConnection(*c);
	dynamic_cast<TCPOut &>(player->out()).setConnection(*c);

	currentGame->processCommand(player);

	// make sure the player wasn't removed from the game before attempting to
	// reset its input and output streams for future use
	if (currentGame->playerIsInGame(request[1])) {
		dynamic_cast<TCPIn &>(player->in()).setConnection(TCPConnection::ptr());
		dynamic_cast<TCPOut &>(player->out()).setConnection(TCPConnection::ptr());
	}

	(*c)->setInUse(false);

	return 0;
}


void COMMANDAction::execute(TCPConnection::ptr connection) {

	vector<string> request = connection->getBufferParts();

	// make sure the command follows the correct syntax ("COMMAND <player>")
	if (2 != request.size()) {
		throw true;
	}

	if (currentGame->playerIsInGame(request[1])) {
		// connection.get() is the raw pointer inside the smart_ptr
		THREAD_CREATE_NONCORE(commandThread, COMMANDAction::processCommandThread,
			connection.get(), "Could not start player command thread.");
	} else {
		connection->write(std::string("NOEXIST") + EOT, freeConnection, 0);
	}

	return;
}

