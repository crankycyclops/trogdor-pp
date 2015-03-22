#include <boost/asio.hpp>

#include "include/network/tcpcommon.h"
#include "include/network/tcpconnection.h"
#include "include/network/tcpserver.h"

#define SERVER_PORT 1040 // config that should be moved outside


void serveConnection(TCPConnection *connection, void *) {

	connection->write("Test", 0, 0);
	connection->close();
}


int main(int argc, char **argv) {

	TCPServer *server;

	while (1) {
		boost::asio::io_service io;
		server = new TCPServer(io, SERVER_PORT);
		server->startAccept(&serveConnection, 0);
		io.run();
		delete server;
	}

	return 0;
}

