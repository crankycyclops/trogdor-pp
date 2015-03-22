#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H


#include <string>

#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include "tcpcommon.h"

using namespace std;
using namespace boost::system;
using boost::asio::ip::tcp;


// Class adapted from the example found here:
// http://www.boost.org/doc/libs/1_54_0/doc/html/boost_asio/tutorial/tutdaytime3/src.html
// I also used the following to implement some missing pieces:
// http://www.boost.org/doc/libs/1_45_0/doc/html/boost_asio/example/timeouts/async_tcp_client.cpp
class TCPConnection {

	private:

		boost::asio::io_service *io;
		tcp::socket *socket;
		boost::asio::streambuf inBuffer;

		// Called after async_read_until() completes. Takes as input a callback
		// function and a void pointer with an argument. Callback is only
		// called if there were no errors during read.
		void handleRead(const error_code &e, callback_t callback, void *callbackArg);

		// Called after async_write() completes. Takes as input a callback
		// function and a void pointer with an argument. Callback is only
		// called if there were no errors during write.
		void handleWrite(const error_code &e, callback_t callback, void *callbackArg);

	public:

		// Constructor (automatically opens a socket)
		inline TCPConnection(boost::asio::io_service &io_service):
			io(&io_service) {socket = new tcp::socket(*io);}

		// Make sure we cleanly close the connection on destruction.
		~TCPConnection() {close();}

		// Closes a socket.
		void close();

		// Return a reference to the socket that represents this connection.
		inline tcp::socket &getSocket() {return *socket;}

		// Initiates an asynchronous read and returns immediately.
		void read(callback_t callback, void *callbackArg);

		// Send a string through a connection that was established during
		// instantiation.
		void write(string message, callback_t callback, void *callbackArg);
};

/******************************************************************************/


#endif

