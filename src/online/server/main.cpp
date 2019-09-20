#include <memory>
#include <iostream>
#include <cstdlib>

#include <boost/asio.hpp>

#include "include/network/tcpcommon.h"
#include "include/network/tcpconnection.h"
#include "include/network/tcpserver.h"

#include "../../core/include/game.h"
#include "../../core/include/iostream/nullout.h"

#define SERVER_PORT 1040 // config that should be moved outside

using namespace std;


// global object that should be accessible to the entire application
trogdor::Game *currentGame;

// serves TCP connections
static TCPServer *server;

/******************************************************************************/

// Called whenever we receive SIGINT (CTRL-C) or SIGTERM
static void shutdownHandler(const boost::system::error_code& error, int signal_number) {

	currentGame->stop();

	delete server;
	delete currentGame;

	cout << "\n" << "Shutting down Trogdor server...\n" << endl;
	exit(error ? EXIT_FAILURE : EXIT_SUCCESS);
}

/******************************************************************************/

int main(int argc, char **argv) {

	/******************************\
    * Step 1. Bootstrap the game *
   \******************************/

	// default game filename
	string gameXML = "game-server.xml";

	if (argc > 2) {
		cerr << "\nUsage: trogdor-server [game_filename.xml]\n\n" << endl;
		return EXIT_FAILURE;
	}

	// user passed in a custom game filename
	if (argc > 1) {
		gameXML = argv[1];
	}

	// TODO: replace NullOut with some kind of logger stream that outputs
	// errors to a file
	std::shared_ptr<NullOut> errStream = make_shared<NullOut>();
	std::unique_ptr<trogdor::Game> currentGame = make_unique<trogdor::Game>(errStream);

	if (currentGame->initialize(gameXML)) {
		currentGame->start();
	} else {
		cerr << "\nFailed to initialize the game :'(\n\n" << endl;
		return EXIT_FAILURE;
	}

	/********************************\
    * Step 2. Bootstrap the server *
   \********************************/

	boost::asio::io_service io;

	// shutdown the server if we receive CTRL-C or a kill signal
	boost::asio::signal_set signals(io, SIGINT, SIGTERM);
	signals.async_wait(shutdownHandler);

	// constructor starts up a deadline_timer that checks at regular intervals
	// on the needs of existing connections as well as accepting new ones.
	server = new TCPServer(io, SERVER_PORT);
	io.run();

	// we don't actually ever get here...
	return EXIT_SUCCESS;
}

