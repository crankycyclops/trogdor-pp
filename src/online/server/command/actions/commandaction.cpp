#include <string>
#include <boost/utility/addressof.hpp>

#include "../../../../core/include/game.h"
#include "../../../../core/include/entities/player.h"

#include "../../include/command/actions/commandaction.h"
#include "../../include/main.h"


void *COMMANDAction::processCommandThread(void *connection) {

	TCPConnection::ptr *c = ((TCPConnection::ptr *)connection);
	vector<string> request = (*c)->getBufferParts();
	core::entity::Player *player = currentGame->getPlayer(request[1]);

	currentGame->processCommand(player);
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
		THREAD_CREATE_NONCORE(commandThread, COMMANDAction::processCommandThread,
			boost::addressof(connection), "Could not start player command thread.");
	} else {
		connection->write(std::string("NOEXIST") + EOT, freeConnection, 0);
	}

	return;
}

