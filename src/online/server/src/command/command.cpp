#include "../include/command/command.h"
#include "../include/command/dispatcher.h"


void serveConnection(TCPConnection::ptr connection, void *) {

	Dispatcher::get()->dispatch(connection);

	// Since we're working asynchronously, we should accept
	// new connections as often as possible.
	connection->getServer()->startAccept(&serveConnection, 0);
}
