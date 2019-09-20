#include <string>
#include <thread>
#include <boost/utility/addressof.hpp>

#include "../../../../core/include/game.h"
#include "../../../../core/include/entities/player.h"

#include "../../include/command/actions/commandaction.h"
#include "../../include/iostream/tcpin.h"
#include "../../include/iostream/tcpout.h"

#include "../../include/main.h"


void *COMMANDAction::processCommandThread(void *connection) {

	TCPConnection::ptr *c = ((TCPConnection::ptr *)connection);
	trogdor::entity::Player *player = (*c)->getPlayer();

	dynamic_cast<TCPIn &>(player->in()).setConnection(*c);
	dynamic_cast<TCPOut &>(player->out()).setConnection(*c);

	currentGame->processCommand(player);

	// make sure the player wasn't removed from the game before attempting to
	// reset its input and output streams for future use
	if (currentGame->playerIsInGame(player->getName())) {
		dynamic_cast<TCPIn &>(player->in()).setConnection(TCPConnection::ptr());
		dynamic_cast<TCPOut &>(player->out()).setConnection(TCPConnection::ptr());
	}

	(*c)->setInUse(false);

	return 0;
}


void COMMANDAction::execute(TCPConnection::ptr connection) {

	vector<string> request = connection->getBufferParts();

	// make sure the command follows the correct syntax ("COMMAND")
	if (1 != request.size()) {
		throw true;
	}

	
	if (connection->getPlayer()) {
		// connection.get() is the raw pointer inside the smart_ptr
		// TODO: need a thread for each connection, since we recycle the
		// CommandAction object. Do I want to use an unordered_map to map
		// connections to individual commandThreads?       
		std::thread processCommand(COMMANDAction::processCommandThread, connection.get());
		processCommand.detach();
	} else {
		connection->write(std::string("AUTHREQ") + EOT, freeConnection, 0);
	}

	return;
}
