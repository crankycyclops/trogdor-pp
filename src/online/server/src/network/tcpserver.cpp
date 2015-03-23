#include <string>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "../include/network/tcpconnection.h"
#include "../include/network/tcpserver.h"
#include "../include/command/command.h"

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

	// TODO: handle existing connections

	// listen for new connections
	startAccept(&serveConnection, 0);

	// call this again at the next interval
	timer.expires_at(timer.expires_at() + boost::posix_time::milliseconds(1000));
	timer.async_wait(boost::bind(&TCPServer::serveConnections, this));
}

