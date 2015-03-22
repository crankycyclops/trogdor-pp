#include <string>

#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include "../include/network/tcpcommon.h"
#include "../include/network/tcpconnection.h"

using namespace std;
using namespace boost::system;
using boost::asio::ip::tcp;


void TCPConnection::handleRead(
	const error_code &e,
	callback_t callback,
	void *callbackArg
) {

	if (!e) {
		if (callback) {
			callback(this, callbackArg);
		}
	}

	// TODO: error handling
}


void TCPConnection::handleWrite(
	const error_code &e,
	callback_t callback,
	void *callbackArg
) {

	if (!e) {
		if (callback) {
			callback(this, callbackArg);
		}
	}

	// TODO: error handling
}


void TCPConnection::close() {

	// Do nothing, because the connection's already closed.
	if (!socket) {
		return;
	}

	boost::system::error_code ec;

	socket->shutdown(tcp::socket::shutdown_send, ec);
	if (ec) {
		// TODO: some error occured. Handle it.
	}

	socket->close();
	delete socket;
	socket = 0;
}


void TCPConnection::read(callback_t callback, void *callbackArg) {

	if (!socket) {
		throw string("Trying to read from a closed connection.");
	}

	boost::asio::async_read_until(
		*socket,
		inBuffer,
		EOT,
		boost::bind(
			&TCPConnection::handleRead,
			this,
			boost::asio::placeholders::error,
			callback,
			callbackArg
		)
	);
}


void TCPConnection::write(string message, callback_t callback, void *callbackArg) {

	if (!socket) {
		throw string("Trying to write on a closed connection.");
	}

	boost::asio::async_write(
		*socket,
		boost::asio::buffer(message),
		boost::bind(
			&TCPConnection::handleWrite,
			this,
			boost::asio::placeholders::error,
			callback,
			callbackArg
		)
	);
}

