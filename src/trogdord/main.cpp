#include <memory>
#include <iostream>
#include <cstdlib>

#include <asio.hpp>

#include "include/config.h"
#include "include/gamecontainer.h"
#include "include/io/input/listenercontainer.h"

#include "include/network/tcpcommon.h"
#include "include/network/tcpconnection.h"
#include "include/network/tcpserver.h"


static const char *STARTUP_MESSAGE = "Starting Trogdord.";
static const char *SHUTDOWN_MESSAGE = "Shutting down Trogdord.";

// serves TCP connections
static std::unique_ptr<TCPServer> server;

/******************************************************************************/

// Called whenever we receive SIGINT (CTRL-C) or SIGTERM.
static void shutdownHandler(const asio::error_code &error, int signal_number) {

	// If auto-dump is enabled, this will save the server's state to disk,
	// including all games, before shutting down.
	if (Config::get()->getBool(Config::CONFIG_KEY_STATE_DUMP_SHUTDOWN_ENABLED)) {
		GameContainer::get()->dump();
	}

	// Forces the server's destructor to be called, ensuring that any remaining
	// connections are closed and that any other cleanup is performed before we
	// exit.
	server = nullptr;

	Config::get()->err(trogdor::Trogerr::INFO) << SHUTDOWN_MESSAGE << std::endl;
	exit(error ? EXIT_FAILURE : EXIT_SUCCESS);
}

/******************************************************************************/

// Called whenever we crash due to something other than an uncaught exception.
static void crashHandler(const asio::error_code &error, int signal_number) {

	// If crash recovery is enabled, this will save the server's state to disk,
	// including all games, before shutting down. It's questionable as to
	// whether or not this will work as intended.
	if (Config::get()->getBool(Config::CONFIG_KEY_STATE_CRASH_RECOVERY_ENABLED)) {
		GameContainer::get()->dump();
	}
}

/******************************************************************************/

int main(int argc, char **argv) {

	std::unique_ptr<Config> &config = Config::get();

	try {

		config->err(trogdor::Trogerr::INFO) << STARTUP_MESSAGE << std::endl;

		// Calling GameContainer::get() invokes the constructor, which then sets up
		// state management for the server.
		GameContainer::get();

		// Retrieving the singleton instance of ListenerContainer will
		// make sure it gets initialized and that the listeners get started
		// right away.
		input::ListenerContainer::get();

		asio::io_service io;

		// Shutdown the server if we receive CTRL-C or a kill signal.
		asio::signal_set shutdownSignals(io, SIGINT, SIGTERM, SIGHUP);
		shutdownSignals.async_wait(shutdownHandler);

		// Attempt crash recovery (if feature is enabled.) I tested with other
		// signals like SIGFPE, SIGSEGV, etc, but those handlers just hang, so
		// I don't think I can reasonably expect to catch those on the way down.
		asio::signal_set crashSignals(io, SIGQUIT);
		crashSignals.async_wait(crashHandler);

		// Constructor starts up a deadline_timer that checks at regular intervals
		// on the needs of existing connections as well as accepting new ones.
		server = std::make_unique<TCPServer>(io, config->getUInt(Config::CONFIG_KEY_PORT));
		io.run();
	}

	catch (std::exception &e) {

		config->err() << e.what() << std::endl;

		// If enabled, dump out server state before exiting due to exception
		if (Config::get()->getBool(Config::CONFIG_KEY_STATE_CRASH_RECOVERY_ENABLED)) {
			GameContainer::get()->dump();
		}

		// Make sure destructors are called and everything shuts down cleanly
		server = nullptr;
		config->err(trogdor::Trogerr::INFO) << SHUTDOWN_MESSAGE << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
