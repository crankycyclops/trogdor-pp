#include <string>
#include "../include/command/networkaction.h"


void NetworkAction::freeConnection(TCPConnection::ptr connection, void *) {

	connection->setInUse(false);
	return;
}
