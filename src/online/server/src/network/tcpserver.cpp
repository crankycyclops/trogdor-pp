#include <string>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "../include/network/tcpconnection.h"
#include "../include/network/tcpserver.h"

using namespace std;
using namespace boost::system;
using boost::asio::ip::tcp;


void TCPServer::handleAccept(
	TCPConnection::ptr connection,
	const error_code &e,
	TCPConnection::callback_t callback,
	void *callbackArg
) {
	if (!e) {
		if (callback) {
			callback(connection, callbackArg);
		}
	}
}


void TCPServer::startAccept(TCPConnection::callback_t callback, void *callbackArg) {

	TCPConnection::ptr connection = TCPConnection::create(acceptor.get_io_service(), this);

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

