#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H


#include <string>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "tcpcommon.h"

using namespace std;
using namespace boost::system;
using boost::asio::ip::tcp;

class TCPServer;


// Class adapted from the example found here:
// http://www.boost.org/doc/libs/1_54_0/doc/html/boost_asio/tutorial/tutdaytime3/src.html
// I also used the following to implement some missing pieces:
// http://www.boost.org/doc/libs/1_45_0/doc/html/boost_asio/example/timeouts/async_tcp_client.cpp
class TCPConnection: public boost::enable_shared_from_this<TCPConnection> {

	public:

		typedef boost::shared_ptr<TCPConnection> ptr;

		// Callback that takes a single argument and is called at the
		// conclusion of an asynchronous event. You can daisychain multiple
		// callbacks by passing additional callbacks to TCPConnection::read()
		// or TCPConnection::write(). Pass in NULL pointers to callback and 
		// arg to signal the end.
		typedef void (*callback_t)(ptr connection, void *arg);

	private:

		TCPServer *server; // server class that spawned this connection
		tcp::socket socket;
		boost::asio::streambuf inBuffer;
		bool inUse;

		// Constructor should only be called internally by create().
		TCPConnection(boost::asio::io_service &io_service, TCPServer *s):
			socket(io_service), server(s), inUse(false) {}

		// Called after async_read_until() completes. Takes as input a callback
		// function and a void pointer with an argument. Callback is only
		// called if there were no errors during read.
		void handleRead(const error_code &e, callback_t callback, void *callbackArg);

		// Called after async_write() completes. Takes as input a callback
		// function and a void pointer with an argument. Callback is only
		// called if there were no errors during write.
		void handleWrite(const error_code &e, callback_t callback, void *callbackArg);

	public:

		// Call this instead of using new directly. Returns a smart pointer
		// to an instance of TCPConnection that will automatically destruct
		// when we're done with it.
		static inline ptr create(boost::asio::io_service &io_service, TCPServer *s) {

			return ptr(new TCPConnection(io_service, s));
		}

		// Returns whether or not the connection is in use (AKA waiting on the
		// completion of an asynchronous call.)
		inline bool isInUse() {return inUse;}

		// Signal whether or not the connection is in use.
		inline void setInUse(bool status) {inUse = status;}

		// Returns a pointer to the server object that spawned this connection
		inline TCPServer *getServer() {return server;}

		// Return a reference to the socket that represents this connection.
		inline tcp::socket &getSocket() {return socket;}

		// Initiates an asynchronous read and returns immediately.
		void read(TCPConnection::callback_t callback, void *callbackArg);

		// Send a string through a connection that was established during
		// instantiation.
		void write(string message, TCPConnection::callback_t callback, void *callbackArg);
};

/******************************************************************************/


#endif
