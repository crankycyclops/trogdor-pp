#include <string>
#include <functional>
#include <chrono>

#include "../include/network/tcpconnection.h"
#include "../include/network/tcpserver.h"
#include "../include/dispatcher.h"
#include "../include/config.h"


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

	connection->write(Dispatcher::get()->dispatch(connection, connection->getBufferStr()) + EOT,
	[] (std::shared_ptr<TCPConnection> connection, void *) {
		connection->setInUse(false);
	}, 0);
}

/******************************************************************************/

TCPServer::TCPServer(asio::io_service &io_service, unsigned short port):
acceptor(io_service), timer(io_service, std::chrono::milliseconds(SERVE_SLEEP_TIME)) {

	std::unique_ptr<Config> &config = Config::get();

	// TODO: enable ipv6 via a second socket. Don't use an ipv6 socket for
	// both protocols (See: https://stackoverflow.com/a/31126262/4683164)
	// I'll need an acceptor for each endpoint, which means I'll have to be
	// able to manage two acceptors instead of one (but I can share the same
	// io_service.)
	// Whether or not to enable ipv6 should be configurable. Also, the port
	// for ipv4 and ipv6 should be configured independently.
	asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), port);
	acceptor.open(endpoint.protocol());

	acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(
		config->value<bool>(Config::CONFIG_KEY_REUSE_ADDRESS)
	));
	acceptor.set_option(asio::ip::tcp::acceptor::keep_alive(
		config->value<bool>(Config::CONFIG_KEY_SEND_TCP_KEEPALIVE)
	));

	acceptor.bind(endpoint);
	acceptor.listen();

	timer.async_wait(std::bind(&TCPServer::serveConnections, this));
}

/******************************************************************************/

TCPServer::~TCPServer() {

	// Close the acceptor to new connections
	if (acceptor.is_open()) {
		acceptor.close();
	}

	// Makes sure the destructor is called on all remaining connections
	activeConnections.clear();
}

/******************************************************************************/

void TCPServer::handleAccept(
	std::shared_ptr<TCPConnection> connection,
	const asio::error_code &e,
	TCPConnection::callback_t callback,
	void *callbackArg
) {
	if (!e) {

		connection->log(trogdor::Trogerr::INFO, "connected.");

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
			std::placeholders::_1,
			callback,
			callbackArg
		)
	);
}

/******************************************************************************/

void TCPServer::serveConnections() {

	// listen for new requests on already accepted connections
	for (std::list<std::shared_ptr<TCPConnection>>::iterator i = activeConnections.begin();
	i != activeConnections.end(); ++i) {

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
	timer.expires_at(timer.expires_at() + std::chrono::milliseconds(SERVE_SLEEP_TIME));
	timer.async_wait(std::bind(&TCPServer::serveConnections, this));
}
