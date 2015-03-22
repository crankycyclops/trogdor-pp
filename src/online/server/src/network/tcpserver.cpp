#include <string>

#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include "../include/network/tcpconnection.h"
#include "../include/network/tcpserver.h"

using namespace std;
using namespace boost::system;
using boost::asio::ip::tcp;


void TCPServer::handleAccept(
	TCPConnection *connection,
	const error_code &e,
	callback_t callback,
	void *callbackArg
) {

	if (!e) {
		callback(connection, callbackArg);
	}
}


void TCPServer::startAccept(callback_t callback, void *callbackArg) {

	TCPConnection *connection = new TCPConnection(acceptor.get_io_service());

	acceptor.async_accept(
		connection->getSocket(),
		boost::bind(
			&TCPServer::handleAccept,
			this,
			connection,
			boost::asio::placeholders::error,
			callback,
			callbackArg
		)
	);
}

