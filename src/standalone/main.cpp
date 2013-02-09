#include <iostream>
#include <cstdlib>

#include "../core/include/game.h"

using namespace std;

int main(int argc, char **argv) {

   core::Game *currentGame = new core::Game();

   currentGame->initialize();

   currentGame->start();
   while (currentGame->inProgress()) {
      currentGame->processCommand();
   }

   cout << "Goodbye!\n\n";
   delete currentGame;
   return EXIT_SUCCESS;
}

