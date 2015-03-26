#include <iostream>
#include <cstdlib>
#include <boost/asio.hpp>

#include "include/network/tcpcommon.h"
#include "include/network/tcpconnection.h"
#include "include/network/tcpserver.h"

#include "../../core/include/game.h"
#include "../../core/include/iostream/streamout.h"
#include "../../core/include/iostream/streamin.h"

#define SERVER_PORT 1040 // config that should be moved outside

using namespace std;


// global object that should be accessible to the entire application
core::Game *currentGame;


int main(int argc, char **argv) {

	// default game filename
	string gameXML = "game-server.xml";

	if (argc > 2) {
		cerr << "\nUsage: trogdor [game_filename.xml]\n\n" << endl;
		return EXIT_FAILURE;
	}

	// user passed in a custom game filename
	if (argc > 1) {
		gameXML = argv[1];
	}

	currentGame = new core::Game();

	if (currentGame->initialize(gameXML)) {
		currentGame->start();
	} else {
		cerr << "\nFailed to initialize the game :'(\n\n" << endl;
		return EXIT_FAILURE;
	}

	boost::asio::io_service io;
	TCPServer *server;

	// constructor starts up a deadline_timer that checks at regular intervals
	// on the needs of existing connections as well as accepting new ones.
	server = new TCPServer(io, SERVER_PORT);
	io.run();

	currentGame->stop();
	delete server;
	return 0;
}

