#include <string>
#include <boost/lexical_cast.hpp>

#include "../../include/network/tcpserver.h"
#include "../../include/command/actions/newplayeraction.h"

#include "../../include/iostream/tcpin.h"
#include "../../include/iostream/tcpout.h"
#include "../../../../core/include/iostream/nullout.h"

// provides extern definition of Game object
#include "../../include/main.h"


void NEWPLAYERAction::execute(TCPConnection::ptr connection) {

	vector<string> request = connection->getBufferParts();

	// check command syntax
	if (request.size() != 2) {
		throw true;
	}

	// add the player and signal to the client that creation was successful
	try {
		// TODO: create DEBUG define statement, and set NullOut to TCPOut in the
		// event that DEBUG is set (will help tremendously in testing; actions do
		// use player->err() to flag bugs.)
		currentGame->createPlayer(request[1], new TCPOut(connection),
			new TCPIn(connection), new core::NullOut());
		connection->getServer()->assignPlayerToConnection(currentGame->getPlayer(request[1]), connection);
		connection->write(std::string("OK") + EOT, freeConnection, 0);
	}

	// player already exists; signal as much to the client
	catch (std::string e) {
		connection->write(std::string("DUP") + EOT, freeConnection, 0);
	}

	return;
}

