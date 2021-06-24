#include <string>
#include <functional>
#include <iostream>

#include <asio.hpp>

#include <trogdord/network/tcpcommon.h>
#include <trogdord/network/tcpconnection.h>
#include <trogdord/network/tcpserver.h>


TCPConnection::TCPConnection(asio::io_service &io_service, TCPServer *s):
server(s), socket(io_service), inUse(false) {}

/******************************************************************************/

TCPConnection::~TCPConnection() {

	// Make sure the actual connection is closed cleanly before destroying the
	// object that wraps around it.
	close();
}

/******************************************************************************/

void TCPConnection::handleRead(
	const asio::error_code &e,
	callback_t callback,
	void *callbackArg
) {
	if (asio::error::eof == e || asio::error::connection_reset == e) {
		log(trogdor::Trogerr::INFO, "disconnected.");
	}

	else if (!e) {

		// const char *bufferData = asio::buffer_cast<const char *>(inBuffer.data());
		bufferStr = asio::buffer_cast<const char *>(inBuffer.data());

		// Clearing the actual streambuf frees it up for later requests.
		clearBuffer();

		// If a callback was passed, call it.
		if (callback) {
			callback(shared_from_this(), callbackArg);
		}
	}
}

/******************************************************************************/

void TCPConnection::handleWrite(
	const asio::error_code &e,
	callback_t callback,
	void *callbackArg
) {
	if (!e) {
		if (callback) {
			callback(shared_from_this(), callbackArg);
		}
	}
}

/******************************************************************************/

void TCPConnection::read(callback_t callback, void *callbackArg) {

	asio::async_read_until(
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

	asio::async_write(
		socket,
		asio::buffer(message),
		std::bind(
			&TCPConnection::handleWrite,
			shared_from_this(),
			std::placeholders::_1,
			callback,
			callbackArg
		)
	);
}
