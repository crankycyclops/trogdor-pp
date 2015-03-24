#include <boost/asio.hpp>

#include "include/network/tcpcommon.h"
#include "include/network/tcpconnection.h"
#include "include/network/tcpserver.h"

#define SERVER_PORT 1040 // config that should be moved outside


int main(int argc, char **argv) {

	boost::asio::io_service io;
	TCPServer *server;

	// constructor starts up a deadline_timer that checks at regular intervals
	// on the needs of existing connections as well as accepting new ones.
	server = new TCPServer(io, SERVER_PORT);
	io.run();

	delete server;
	return 0;
}

