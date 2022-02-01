#include <string>
#include <functional>
#include <chrono>

#include <trogdord/network/tcpconnection.h>
#include <trogdord/network/tcpserver.h>
#include <trogdord/dispatcher.h>
#include <trogdord/config.h>


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
timer(io_service, std::chrono::milliseconds(SERVE_SLEEP_TIME)) {

	std::unique_ptr<Config> &config = Config::get();

	rapidjson::Document listenersArr;

	std::string listenersStr = Config::get()->getString(
		Config::CONFIG_KEY_LISTEN_IPS
	);

	listenersArr.Parse(listenersStr.c_str());

	if (listenersArr.HasParseError()) {
		throw ServerException(
			std::string("invalid trogdord.ini value for ") +
			Config::CONFIG_KEY_LISTEN_IPS
		);
	}

	else if (rapidjson::kArrayType != listenersArr.GetType()) {
		throw ServerException(
			std::string("trogdord.ini value for ") +
			Config::CONFIG_KEY_LISTEN_IPS +
			" must be a JSON array of strings."
		);
	}

	for (auto listener = listenersArr.Begin(); listener != listenersArr.End(); listener++) {

		if (rapidjson::kStringType == listener->GetType()) {

			asio::ip::address ipAddress = asio::ip::address::from_string(listener->GetString());

			asio::ip::tcp::endpoint endpoint(ipAddress, port);
			auto acceptor = std::make_unique<asio::ip::tcp::acceptor>(io_service);

			acceptor->open(endpoint.protocol());

			acceptor->set_option(asio::ip::tcp::acceptor::reuse_address(
				config->getBool(Config::CONFIG_KEY_REUSE_ADDRESS)
			));

			acceptor->set_option(asio::ip::tcp::acceptor::keep_alive(
				config->getBool(Config::CONFIG_KEY_SEND_TCP_KEEPALIVE)
			));

			// In the case of :: and 0.0.0.0, this avoids the dual stack
			// port sharing issue explained at the end of this answer:
			// https://superuser.com/a/1667180
			if (ipAddress.is_v6()) {
				acceptor->set_option(asio::ip::v6_only(true));
			}

			acceptor->bind(endpoint);
			acceptor->listen();

			acceptors.push_back(std::move(acceptor));
			config->err(trogdor::Trogerr::INFO) << "Listening on " << listener->GetString() << '.' << std::endl;
		}

		else {
			throw ServerException(
				std::string("trogdord.ini value for ") +
					Config::CONFIG_KEY_LISTEN_IPS +
					" must be a JSON array of strings."
			);
		}
	}

	timer.async_wait(std::bind(&TCPServer::serveConnections, this));
}

/******************************************************************************/

TCPServer::~TCPServer() {

	// Close acceptors to new connections
	for (auto &acceptor: acceptors) {
		if (acceptor->is_open()) {
			acceptor->close();
		}
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

	for (auto &acceptor: acceptors) {

		std::shared_ptr<TCPConnection> connection = TCPConnection::create(
			acceptor->get_io_service(),
			this
		);

		acceptor->async_accept(
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
