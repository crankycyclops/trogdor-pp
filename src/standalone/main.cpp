/*
   This is a sample single player client whose sole purpose is to demo what the
   Trogdor engine (in src/core) can do.  The engine itself supports multiple
   players, and will soon have code that will allow input and output streams
   to function over a network.
*/

#include <iostream>
#include <cstdlib>

#include "../core/include/game.h"
#include "../core/include/iostream/streamout.h"
#include "../core/include/iostream/streamin.h"


using namespace std;

int main(int argc, char **argv) {

   // default game filename
   string gameXML = "game.xml";

   if (argc > 2) {
      cerr << "Usage: trogdor [game_filename.xml]\n" << endl;
      return EXIT_FAILURE;
   }

   // user passed in a custom game filename
   if (argc > 1) {
      gameXML = argv[1];
   }

   core::Game *currentGame = new core::Game();

   if (currentGame->initialize(gameXML)) {

      string title = currentGame->getMeta("title");
      string author = currentGame->getMeta("author");

      cout << endl;

      if (title.length() > 0) {
         cout << endl << "Title: " << title << endl;
      }

      if (author.length() > 0) {
         cout << "Author: " << author << endl << endl;
      }

      Player *player = currentGame->createPlayer("player", new core::StreamOut(&cout),
         new core::StreamIn(&cin), new core::StreamOut(&cerr));

      currentGame->start();

      while (currentGame->inProgress() && currentGame->playerIsInGame("player")) {
         currentGame->processCommand(player);
      }

      currentGame->stop();

      cout << "Game lasted for " << currentGame->getTime() + 1
         << " seconds.  Goodbye!\n\n";
   }

   delete currentGame;
   return EXIT_SUCCESS;
}

