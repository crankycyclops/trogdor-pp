#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H


#include <string>
#include <vector>
#include <memory>

#include <asio.hpp>

#include "../config.h"
#include "tcpcommon.h"


class TCPServer;


// Class adapted from the example found here:
// http://www.boost.org/doc/libs/1_54_0/doc/html/boost_asio/tutorial/tutdaytime3/src.html
// I also used the following to implement some missing pieces:
// http://www.boost.org/doc/libs/1_45_0/doc/html/boost_asio/example/timeouts/async_tcp_client.cpp
class TCPConnection: public std::enable_shared_from_this<TCPConnection> {

	public:

		// Callback that takes a single argument and is called at the
		// conclusion of an asynchronous event. You can daisychain multiple
		// callbacks by passing additional callbacks to TCPConnection::read()
		// or TCPConnection::write(). Pass in NULL pointers to callback and 
		// arg to signal the end.
		typedef void (*callback_t)(std::shared_ptr<TCPConnection> connection, void *arg);

		// Ensures the actual connection is cleanly shutdown when the
		// TCPConnection object is destroyed
		~TCPConnection();

	private:

		TCPServer *server; // server class that spawned this connection
		asio::ip::tcp::socket socket;
		bool inUse;

		asio::streambuf inBuffer;
		std::string bufferStr;

		// Records the client's hostname/IP address for logging
		std::string clientHost = "";

		// Constructor should only be called internally by create().
		TCPConnection(asio::io_service &io_service, TCPServer *s);

		// Called after async_read_until() completes. Takes as input a callback
		// function and a void pointer with an argument. Callback is only
		// called if there were no errors during read.
		void handleRead(const asio::error_code &e, callback_t callback, void *callbackArg);

		// Called after async_write() completes. Takes as input a callback
		// function and a void pointer with an argument. Callback is only
		// called if there were no errors during write.
		void handleWrite(const asio::error_code &e, callback_t callback, void *callbackArg);

		// Clears the buffer once it's been used
		inline void clearBuffer() {inBuffer.consume(bufferStr.length() + 1);}

	public:

		// Call this instead of using new directly. Returns a smart pointer
		// to an instance of TCPConnection that will automatically destruct
		// when we're done with it.
		static inline std::shared_ptr<TCPConnection> create(
			asio::io_service &io_service, TCPServer *s
		) {

			// Calling new instead of using std::make_shared because the
			// constructor is private and can't be called outside our class.
			return std::shared_ptr<TCPConnection>(new TCPConnection(io_service, s));
		}

		// Whether or not the connection is open.
		inline bool isOpen() {return socket.is_open();}

		// Returns whether or not the connection is in use (AKA waiting on the
		// completion of an asynchronous call.)
		inline bool isInUse() {return inUse;}

		// Signal whether or not the connection is in use.
		inline void setInUse(bool status) {inUse = status;}

		// Returns a string containing the last received message.
		inline std::string getBufferStr() const {return bufferStr;}

		// Returns a pointer to the server object that spawned this connection
		inline TCPServer *getServer() {return server;}

		// Return a reference to the socket that represents this connection.
		inline asio::ip::tcp::socket &getSocket() {return socket;}

		// Initiates an asynchronous read and returns immediately.
		void read(TCPConnection::callback_t callback, void *callbackArg);

		// Send a string through a connection that was established during
		// instantiation.
		void write(std::string message, TCPConnection::callback_t callback, void *callbackArg);

		// Logs a message associated with the connection.
		inline void log(const trogdor::Trogerr::ErrorLevel &severity, const std::string &message) {

			// Get and cache the client's hostname/IP only once
			if (!clientHost.length()) {

				try {
					clientHost = socket.remote_endpoint().address().to_string();
				}

				catch (const std::exception &e) {
					clientHost = "(null)";
				}
			}

			Config::get()->err(severity) << clientHost << ": " << message << std::endl;
			return;
		}

		// Closes the connection. Once this is done, the connection cannot be
		// reopened again, and any further operations on this connection
		// object beyond calling isOpen() will result in undefined behavior.
		inline void close() {

			if (socket.is_open()) {
				asio::error_code ignore;
				socket.shutdown(asio::ip::tcp::socket::shutdown_both, ignore);
				socket.close();
			}
		}
};

/******************************************************************************/


#endif
