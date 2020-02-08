#include <string>
#include <functional>

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "../include/network/tcpconnection.h"
#include "../include/network/tcpserver.h"

using namespace boost::system;
using boost::asio::ip::tcp;

// TODO: move this to an appropriate place. I stole it from the old Dispatcher!
static void establishConnection(std::shared_ptr<TCPConnection> connection, void *) {

	// add connection to list of active connections
	connection->getServer()->addActiveConnection(connection);

	// confirm successful connection to the client
	connection->setInUse(true);
	connection->write(std::string(READY) + EOT,
	[] (std::shared_ptr<TCPConnection> connection, void *) {
		connection->setInUse(false);
	}, 0);
}

/******************************************************************************/

// TODO: stub for now, stolen from Dispatcher
static void serveRequest(std::shared_ptr<TCPConnection> connection, void *) {

	std::string request = connection->getBufferStr();
}

/******************************************************************************/

TCPServer::TCPServer(boost::asio::io_service &io_service, unsigned short port):
acceptor(io_service), timer(io_service, boost::posix_time::milliseconds(SERVE_SLEEP_TIME)) {

	tcp::endpoint endpoint(tcp::v4(), port);

	acceptor.open(endpoint.protocol());
	acceptor.set_option(tcp::acceptor::reuse_address(true));
	acceptor.bind(endpoint);
	acceptor.listen();

	timer.async_wait(std::bind(&TCPServer::serveConnections, this));
}

/******************************************************************************/

void TCPServer::handleAccept(
	std::shared_ptr<TCPConnection> connection,
	const boost::system::error_code &e,
	TCPConnection::callback_t callback,
	void *callbackArg
) {
	if (!e) {
		if (callback) {
			callback(connection, callbackArg);
		}
	}
}

/******************************************************************************/

void TCPServer::startAccept(TCPConnection::callback_t callback, void *callbackArg) {

	std::shared_ptr<TCPConnection> connection = TCPConnection::create(
		acceptor.get_io_service(),
		this
	);

	acceptor.async_accept(
		connection->getSocket(),
		std::bind(
			&TCPServer::handleAccept,
			this,
			connection,
			std::placeholders::_1, // replaced boost::placeholders::error
			callback,
			callbackArg
		)
	);
}

/******************************************************************************/

void TCPServer::serveConnections() {

	size_t nConnections = activeConnections.size();

	// listen for new requests on already accepted connections
	for (std::list<std::shared_ptr<TCPConnection>>::iterator i = activeConnections.begin();
	i != activeConnections.end(); i++) {

		std::shared_ptr<TCPConnection> connection = *i;

		// If the connection was closed, remove it and skip ahead to the next.
		if (!connection->isOpen()) {
			i = activeConnections.erase(i);
			continue;
		}

		// If the connection isn't already in the middle of a request, we 
		// should listen for a new one.
		else if (!connection->isInUse()) {
			connection->setInUse(true);
			connection->read(serveRequest, 0);
		}
	}

	// listen for new connections
	startAccept(establishConnection, 0);

	// call this again at the next interval
	timer.expires_at(timer.expires_at() + boost::posix_time::milliseconds(SERVE_SLEEP_TIME));
	timer.async_wait(std::bind(&TCPServer::serveConnections, this));
}
