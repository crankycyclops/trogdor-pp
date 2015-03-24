#include "../include/network/tcpconnection.h"
#include "../include/network/tcpserver.h"
#include "../include/command/dispatcher.h"

#include "../include/command/actions/connectaction.h"

using namespace std;


// static
Dispatcher *Dispatcher::instance;

/******************************************************************************/

// static
Dispatcher *Dispatcher::get() {

	if (!instance) {
		instance = new Dispatcher();
	}

	return instance;
}

/******************************************************************************/

void Dispatcher::establishConnection(TCPConnection::ptr connection, void *) {

	// add connection to list of active connections
	connection->getServer()->addActiveConnection(connection);

	// confirm successful connection to the client
	// TODO: move instantiation to a registry
	ConnectAction action;
	connection->setInUse(true);
	action(connection);
}

/******************************************************************************/

void Dispatcher::serveRequest(TCPConnection::ptr connection, void *) {

	get()->dispatch(connection);
}

/******************************************************************************/

void Dispatcher::dispatch(TCPConnection::ptr connection) {

	// TODO: grab command from connection's buffer, which was already read
	// TODO: this should be part of the NetworkAction
	connection->write("Test\n", 0, 0);
	connection->setInUse(false);

	return;
}

