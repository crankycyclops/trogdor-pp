#ifndef TCPSERVER_H
#define TCPSERVER_H


#define SERVER_PORT 1040

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

#include "tcpcommon.h"
#include "tcpconnection.h"

using namespace boost::system;
using boost::asio::ip::tcp;


// Class adapted from the example found here:
// http://www.boost.org/doc/libs/1_54_0/doc/html/boost_asio/tutorial/tutdaytime3/src.html
// I also used the following to implement some missing pieces:
// http://www.boost.org/doc/libs/1_45_0/doc/html/boost_asio/example/timeouts/async_tcp_client.cpp
class TCPServer {

	private:

		tcp::acceptor acceptor;

		// Handles an asynchronous accept. If there are no errors, the
		// specified callback is called along with the specified argument.
		void handleAccept(TCPConnection::ptr connection, const error_code &e,
			TCPConnection::callback_t callback, void *arg);

	public:

		// Contructor establishes that we're using IPv4 and that we're
		// listening on port SERVER_PORT.
		TCPServer(boost::asio::io_service &io_service):
			acceptor(io_service, tcp::endpoint(tcp::v4(), SERVER_PORT)) {}

		// Calls async_accept(), which waits for a connection in a separate
		// thread. The provided callback should assume that a connection was
		// successfully accepted and should continue from there using the
		// provided TCPConnection.
		void startAccept(TCPConnection::callback_t callback, void *callbackArg);
};


#endif

