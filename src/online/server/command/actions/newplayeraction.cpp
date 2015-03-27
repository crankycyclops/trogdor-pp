#include <string>
#include <boost/lexical_cast.hpp>

#include "../../include/network/tcpserver.h"
#include "../../include/command/actions/newplayeraction.h"

#include "../../../../core/include/iostream/nullin.h"
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
		// TODO: implement network stream objects to replace NullIn/Out
		currentGame->createPlayer(request[1], new core::NullOut(),
		    new core::NullIn(), new core::NullOut());
		connection->write(std::string("OK") + EOT, freeConnection, 0);
	}

	// player already exists; signal as much to the client
	catch (std::string e) {
		connection->write(std::string("DUP") + EOT, freeConnection, 0);
	}

	return;
}

