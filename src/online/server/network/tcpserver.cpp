#include <string>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "../include/network/tcpconnection.h"
#include "../include/network/tcpserver.h"
#include "../include/command/dispatcher.h"

using namespace std;
using namespace boost::system;
using boost::asio::ip::tcp;


TCPServer::TCPServer(boost::asio::io_service &io_service, unsigned short port):
acceptor(io_service), timer(io_service, boost::posix_time::milliseconds(SERVE_SLEEP_TIME)) {

	tcp::endpoint endpoint(tcp::v4(), port);

	acceptor.open(endpoint.protocol());
	acceptor.set_option(tcp::acceptor::reuse_address(true));
	acceptor.bind(endpoint);
	acceptor.listen();

	timer.async_wait(boost::bind(&TCPServer::serveConnections, this));
}

/******************************************************************************/

void TCPServer::assignPlayerToConnection(core::entity::Player *player, TCPConnection::ptr connection) {

	if (playerToConnection.left.find(player) != playerToConnection.left.end()) {
		playerToConnection.left.find(player)->second->close();
		playerToConnection.left.erase(player);
	}

	playerToConnection.insert(PlayerConnectionMap::value_type(player, connection));
}

/******************************************************************************/

void TCPServer::handleAccept(
	TCPConnection::ptr connection,
	const error_code &e,
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

	TCPConnection::ptr connection = TCPConnection::create(acceptor.get_io_service(), this);

	acceptor.async_accept(
		connection->getSocket(),
		boost::bind(
			&TCPServer::handleAccept,
			this,
			connection,
			boost::asio::placeholders::error,
			callback,
			callbackArg
		)
	);
}

/******************************************************************************/

void TCPServer::serveConnections() {

	size_t nConnections = activeConnections.size();

	// listen for new requests on already accepted connections
	for (std::list<TCPConnection::ptr>::iterator i = activeConnections.begin();
	i != activeConnections.end(); i++) {

		TCPConnection::ptr connection = *i;

		// If the connection was closed, remove it.
		if (!connection->isOpen()) {
			removeActiveConnection(connection);
		}

		// If the connection isn't already in the middle of a request, we 
		// should listen for a new one.
		else if (!connection->isInUse()) {
			connection->setInUse(true);
			connection->read(Dispatcher::serveRequest, 0);
		}
	}

	// listen for new connections
	startAccept(Dispatcher::establishConnection, 0);

	// call this again at the next interval
	timer.expires_at(timer.expires_at() + boost::posix_time::milliseconds(SERVE_SLEEP_TIME));
	timer.async_wait(boost::bind(&TCPServer::serveConnections, this));
}

