#ifndef TCPSERVER_H
#define TCPSERVER_H


// defined in milliseconds (tuned for my laptop: YMMV)
// TODO: eventually make this a setting that can be read in via a config file
#define SERVE_SLEEP_TIME 5

#include <list>

#include <asio.hpp>
#include <asio/system_timer.hpp>

#include "tcpcommon.h"
#include "tcpconnection.h"


// Class adapted from the example found here:
// http://www.boost.org/doc/libs/1_54_0/doc/html/boost_asio/tutorial/tutdaytime3/src.html
// I also used the following to implement some missing pieces:
// http://www.boost.org/doc/libs/1_45_0/doc/html/boost_asio/example/timeouts/async_tcp_client.cpp
class TCPServer {

	private:

		// active connections that need to be maintained.
		std::list<std::shared_ptr<TCPConnection>> activeConnections;

		// Each acceptor listens on a different IP address or range and
		// accepts connections when they come in
		std::list<std::unique_ptr<asio::ip::tcp::acceptor>> acceptors;

		// Used to make sure serveConnections() only fires at a given interval.
		asio::system_timer timer;

		// Creates a new instance of TCPConnection and sends it an
		// acknowledgement to let it know it's ready to start accepting
		// requests.
		static void establishConnection(std::shared_ptr<TCPConnection> connection, void *);

		// Processes a request that came in through the socket.
		static void serveRequest(std::shared_ptr<TCPConnection> connection, void *);

		// Handles an asynchronous accept. If there are no errors, the
		// specified callback is called along with the specified argument.
		void handleAccept(
			std::shared_ptr<TCPConnection> connection,
			const asio::error_code &e,
			TCPConnection::callback_t callback,
			void *arg
		);

	public:

		// Adds a connection to the list of active connections
		inline void addActiveConnection(std::shared_ptr<TCPConnection> connection) {

			activeConnections.insert(activeConnections.begin(), connection);
		}

		// Removes a connection from the list of active connections, thereby
		// effectively closing it.
		inline void removeActiveConnection(std::shared_ptr<TCPConnection> connection) {

			activeConnections.remove(connection);
		}

		// Contructor establishes that we're using IPv4 and that we're
		// listening on port SERVER_PORT.
		TCPServer(asio::io_service &io_service, unsigned short port);

		// Makes sure the server is cleanly shutdown
		~TCPServer();

		// Calls async_accept(), which waits for a connection in a separate
		// thread. The provided callback should assume that a connection was
		// successfully accepted and should continue from there using the
		// provided TCPConnection.
		void startAccept(TCPConnection::callback_t callback, void *callbackArg);

		// Executes at regular intervals, serving existing connections and
		// listening for new ones.
		void serveConnections();
};


#endif
