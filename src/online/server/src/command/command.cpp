#include "../include/command/command.h"


void serveConnection(TCPConnection::ptr connection, void *) {

	connection->write("Test\n", 0, 0);
	connection->getServer()->startAccept(&serveConnection, 0);
}

