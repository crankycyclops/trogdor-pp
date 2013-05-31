#include <iostream>
#include <cstdlib>

#include "../core/include/game.h"
#include "../core/include/iostream/consoleout.h"


using namespace std;

int main(int argc, char **argv) {

   core::Game *currentGame = new core::Game();

   if (currentGame->initialize()) {

      Player *player = currentGame->createPlayer("player", new core::ConsoleOut());

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

