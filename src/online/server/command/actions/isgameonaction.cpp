#include <string>
#include "../../include/network/tcpserver.h"
#include "../../include/command/actions/isgameonaction.h"

// provides extern definition of Game object
#include "../../include/main.h"


void ISGAMEONAction::execute(TCPConnection::ptr connection) {

	connection->write(std::string(currentGame->inProgress() ? "YES" : "NO") + EOT, freeConnection, 0);
	return;
}

