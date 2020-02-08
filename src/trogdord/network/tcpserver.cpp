#include <string>
#include <functional>

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "../include/network/tcpconnection.h"
#include "../include/network/tcpserver.h"
#include "../include/dispatcher.h"

using namespace boost::system;
using boost::asio::ip::tcp;


void TCPServer::establishConnection(std::shared_ptr<TCPConnection> connection, void *) {

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

void TCPServer::serveRequest(std::shared_ptr<TCPConnection> connection, void *) {

	Dispatcher::get()->dispatch(connection->getBufferStr());
	connection->setInUse(false);
}

/******************************************************************************/

TCPServer::TCPServer(boost::asio::io_service &io_service, unsigned short port):
acceptor(io_service), timer(io_service, boost::posix_time::milliseconds(SERVE_SLEEP_TIME)) {

	// TODO: enable ipv6 via a second socket. Don't use an ipv6 socket for
	// both protocols (See: https://stackoverflow.com/a/31126262/4683164)
	// I'll need an acceptor for each endpoint, which means I'll have to be
	// able to manage two acceptors instead of one (but I can share the same
	// io_service.)
	// Whether or not to enable ipv6 should be configurable. Also, the port
	// for ipv4 and ipv6 should be configured independently.
	tcp::endpoint endpoint(tcp::v4(), port);
	acceptor.open(endpoint.protocol());

	// TODO: make these options configurable via a file in /etc
	acceptor.set_option(tcp::acceptor::reuse_address(true));
	acceptor.set_option(tcp::acceptor::keep_alive(true));

	acceptor.bind(endpoint);
	acceptor.listen();

	timer.async_wait(std::bind(&TCPServer::serveConnections, this));
}

/******************************************************************************/

TCPServer::~TCPServer() {

	// Makes sure the destructor is called on all remaining connections
	activeConnections.clear();
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
