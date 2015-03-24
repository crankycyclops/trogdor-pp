#include <string>
#include "../../include/command/actions/connectaction.h"


void ConnectAction::acknowledgeCallback(TCPConnection::ptr connection, void *) {

	connection->setInUse(false);
}


void ConnectAction::execute(TCPConnection::ptr connection) {

	connection->write(std::string(ACKNOWLEDGE) + EOT, acknowledgeCallback, 0);
	return;
}

