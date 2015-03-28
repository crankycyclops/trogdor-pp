#include "../include/network/tcpconnection.h"
#include "../include/network/tcpserver.h"
#include "../include/command/dispatcher.h"

#include "../include/command/actions/connectaction.h"
#include "../include/command/actions/commandaction.h"
#include "../include/command/actions/badcommandaction.h"
#include "../include/command/actions/isgameonaction.h"
#include "../include/command/actions/timeaction.h"
#include "../include/command/actions/newplayeraction.h"

using namespace std;


// static
Dispatcher *Dispatcher::instance = 0;

/******************************************************************************/

// static
Dispatcher *Dispatcher::get() {

	if (!instance) {
		instance = new Dispatcher();
	}

	return instance;
}

/******************************************************************************/

Dispatcher::Dispatcher() {

	// register possible commands and their corresponding actions.
	actions["ISGAMEON"]  = new ISGAMEONAction();
	actions["TIME"]      = new TIMEAction();
	actions["NEWPLAYER"] = new NEWPLAYERAction();
	actions["COMMAND"]   = new COMMANDAction();

	// Special NetworkAction we use to confirm to the client that a connection
	// has been made
	confirmConnect = new ConnectAction();

	// special NetworkAction to throw when client provides an unknown command
	badCommand = new BadCommandAction();
}

/******************************************************************************/

Dispatcher::~Dispatcher() {

	for (ActionMap::iterator i = actions.begin(); i != actions.end(); i++) {
		delete i->second;
	}

	delete badCommand;
	delete confirmConnect;
}

/******************************************************************************/

NetworkAction *Dispatcher::getAction(const string command) const {

	if (actions.find(command) == actions.end()) {
		return badCommand;
	}

	return actions.find(command)->second;
}

/******************************************************************************/

void Dispatcher::destroy() {

	if (instance) {
		delete instance;
		instance = 0;
	}
}

/******************************************************************************/

void Dispatcher::establishConnection(TCPConnection::ptr connection, void *) {

	// add connection to list of active connections
	connection->getServer()->addActiveConnection(connection);

	// confirm successful connection to the client
	connection->setInUse(true);
	(*get()->confirmConnect)(connection);
}

/******************************************************************************/

void Dispatcher::serveRequest(TCPConnection::ptr connection, void *) {

	get()->dispatch(connection);
}

/******************************************************************************/

void Dispatcher::dispatch(TCPConnection::ptr connection) {

	// process tokenized request
	vector<string> request = connection->getBufferParts();

	// action throws an exception if the command syntax is invalid
	try {
		(*get()->getAction(request[0]))(connection);
	} catch (bool e) {
		(*get()->badCommand)(connection);
	}

	return;
}

