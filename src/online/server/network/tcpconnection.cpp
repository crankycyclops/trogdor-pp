#include <string>

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

