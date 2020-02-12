#include <memory>
#include <iostream>
#include <cstdlib>

#include <boost/asio.hpp>

#include "include/config.h"
#include "include/network/tcpcommon.h"
#include "include/network/tcpconnection.h"
#include "include/network/tcpserver.h"
#include "include/iostream/streamerr.h"


// serves TCP connections
static std::unique_ptr<TCPServer> server;

/******************************************************************************/

// Called whenever we receive SIGINT (CTRL-C) or SIGTERM.
static void shutdownHandler(const boost::system::error_code& error, int signal_number) {

	// Forces the server's destructor to be called, ensuring that any remaining
	// connections are closed and that any other cleanup is performed before we
	// exit.
	server = nullptr;

	Config::get()->err(trogdor::Trogerr::INFO) << "Shutting down Trogdord." << std::endl;
	exit(error ? EXIT_FAILURE : EXIT_SUCCESS);
}

/******************************************************************************/

int main(int argc, char **argv) {

	std::unique_ptr<Config> &config = Config::get();
	config->err(trogdor::Trogerr::INFO) << "Starting Trogdord." << std::endl;

	try {

		boost::asio::io_service io;

		// Shutdown the server if we receive CTRL-C or a kill signal.
		boost::asio::signal_set signals(io, SIGINT, SIGTERM);
		signals.async_wait(shutdownHandler);

		// Constructor starts up a deadline_timer that checks at regular intervals
		// on the needs of existing connections as well as accepting new ones.
		server = std::make_unique<TCPServer>(io, config->value<int>(Config::CONFIG_KEY_PORT));
		io.run();
	}

	catch (std::exception &e) {
		config->err() << e.what() << std::endl;
	}

	return EXIT_SUCCESS;
}
