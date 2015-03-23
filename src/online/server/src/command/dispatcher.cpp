#include "../include/network/tcpconnection.h"
#include "../include/command/dispatcher.h"

using namespace std;


// static
Dispatcher *Dispatcher::instance;

// static
Dispatcher *Dispatcher::get() {

	if (!instance) {
		instance = new Dispatcher();
	}

	return instance;
}


void Dispatcher::dispatch(TCPConnection::ptr connection) {

	// TODO
	connection->write("Test\n", 0, 0);
	return;
}
