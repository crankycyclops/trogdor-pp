#include "../../../../core/include/entities/player.h"
#include "../../include/command/actions/loginaction.h"

#include "../../include/main.h"


void LOGINAction::execute(TCPConnection::ptr connection) {

	vector<string> request = connection->getBufferParts();

	// make sure the command follows the correct syntax ("LOGIN <player name>")
	if (2 != request.size()) {
		throw true;
	}

	try {
		connection->assignPlayer(currentGame->getPlayer(request[1]));
		connection->write(std::string("OK") + EOT, freeConnection, 0);
	} catch (string e) {
		connection->write(std::string("NOEXIST") + EOT, freeConnection, 0);
	}

	return;
}
