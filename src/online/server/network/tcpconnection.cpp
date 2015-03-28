#include <string>
#include <iostream>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
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

		// copy the contents of the buffer to a string, trim out EOT and clear
		// the buffer for the next request
		const char *bufferData = boost::asio::buffer_cast<const char *>(inBuffer.data());

		// A little hacky. Basically, every time new data is written into the
		// streambuffer, the string inside isn't actually overwritten with '\0's.
		// As a result, smaller strings that come in later, terminated in EOT,
		// still contain the remainder of the previously received longer string,
		// which of course produces weird undefined behavior. My workaround,
		// whether or not it's the right way, is to replace EOT with '\0',
		// which will result in the creation of a new valid C-style string.
		char *p = (char *)bufferData;
		for (; *p != EOT; p++);
		*p = '\0';

		// copy the corrected streambuf string into std::string bufferStr
		bufferStr = bufferData;

		// clearing the actual streambuf frees it up for later requests
		clearBuffer();

		// if a callback was passed, call it
		if (callback) {
			callback(shared_from_this(), callbackArg);
		}
	}

	// TODO: error handling
}

/******************************************************************************/

void TCPConnection::handleWrite(
	const error_code &e,
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
		boost::bind(
			&TCPConnection::handleRead,
			shared_from_this(),
			boost::asio::placeholders::error,
			callback,
			callbackArg
		)
	);
}

/******************************************************************************/

void TCPConnection::write(string message, callback_t callback, void *callbackArg) {

	boost::asio::async_write(
		socket,
		boost::asio::buffer(message),
		boost::bind(
			&TCPConnection::handleWrite,
			shared_from_this(),
			boost::asio::placeholders::error,
			callback,
			callbackArg
		)
	);
}

