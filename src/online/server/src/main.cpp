#include <boost/asio.hpp>

#include "include/network/tcpcommon.h"
#include "include/network/tcpconnection.h"
#include "include/network/tcpserver.h"

#define SERVER_PORT 1040 // config that should be moved outside


void serveConnection(TCPConnection::ptr connection, void *) {

	connection->write("Test", 0, 0);
}


int main(int argc, char **argv) {

	boost::asio::io_service io;
	TCPServer *server = new TCPServer(io, SERVER_PORT);

	server->startAccept(&serveConnection, 0);
	io.run(); // runs forever

	delete server;
	return 0;
}

