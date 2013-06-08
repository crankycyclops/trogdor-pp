#include <iostream>
#include <cstdlib>

#include "../core/include/game.h"
#include "../core/include/iostream/streamout.h"
#include "../core/include/iostream/streamin.h"


using namespace std;

int main(int argc, char **argv) {

   core::Game *currentGame = new core::Game();

   if (currentGame->initialize()) {

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

