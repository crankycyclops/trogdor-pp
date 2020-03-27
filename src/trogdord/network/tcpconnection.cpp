#include <string>
#include <functional>
#include <iostream>

#include <boost/asio.hpp>

#include "../include/network/tcpcommon.h"
#include "../include/network/tcpconnection.h"
#include "../include/network/tcpserver.h"

using namespace boost::system;
using boost::asio::ip::tcp;


TCPConnection::TCPConnection(boost::asio::io_service &io_service, TCPServer *s):
server(s), socket(io_service), inUse(false) {}

/******************************************************************************/

TCPConnection::~TCPConnection() {

	// Make sure the actual connection is closed cleanly before destroying the
	// object that wraps around it.
	close();
}

/******************************************************************************/

void TCPConnection::handleRead(
	const boost::system::error_code &e,
	callback_t callback,
	void *callbackArg
) {
	if (boost::asio::error::eof == e || boost::asio::error::connection_reset == e) {
		log(trogdor::Trogerr::INFO, "disconnected.");
	}

	else if (!e) {

		// const char *bufferData = boost::asio::buffer_cast<const char *>(inBuffer.data());
		bufferStr = boost::asio::buffer_cast<const char *>(inBuffer.data());

		// Clearing the actual streambuf frees it up for later requests.
		clearBuffer();

		// If a callback was passed, call it.
		if (callback) {
			callback(shared_from_this(), callbackArg);
		}
	}

	// TODO: error handling
}

/******************************************************************************/

void TCPConnection::handleWrite(
	const boost::system::error_code &e,
	callback_t callback,
	void *callbackArg
) {
	if (!e) {
		if (callback) {
			callback(shared_from_this(), callbackArg);
		}
	}

	// TODO: error handling
}

/******************************************************************************/

void TCPConnection::read(callback_t callback, void *callbackArg) {

	boost::asio::async_read_until(
		socket,
		inBuffer,
		EOT,
		std::bind(
			&TCPConnection::handleRead,
			shared_from_this(),
			std::placeholders::_1,
			callback,
			callbackArg
		)
	);
}

/******************************************************************************/

void TCPConnection::write(std::string message, callback_t callback, void *callbackArg) {

	boost::asio::async_write(
		socket,
		boost::asio::buffer(message),
		std::bind(
			&TCPConnection::handleWrite,
			shared_from_this(),
			std::placeholders::_1,
			callback,
			callbackArg
		)
	);
}