#include <iostream>
#include <cstdlib>

#include "../core/include/game.h"
#include "../core/include/luastate.h"

using namespace std;

int main(int argc, char **argv) {

   core::Game *currentGame = new core::Game();

   if (currentGame->initialize()) {

      currentGame->start();
      while (currentGame->inProgress()) {
         currentGame->processCommand();
      }

      cout << "Game lasted for " << currentGame->getTime() + 1
         << " seconds.  Goodbye!\n\n";
   }

   delete currentGame;
   return EXIT_SUCCESS;
}

