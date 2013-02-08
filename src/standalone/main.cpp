using namespace std;

#include <cstdlib>

#include "../core/include/game.h"

int main(int argc, char **argv) {

   core::Game *currentGame = new core::Game();

   currentGame->initialize();

   currentGame->start();
   while (currentGame->inProgress()) {
      // TODO
      return EXIT_SUCCESS;
   }

   delete currentGame;
   return EXIT_SUCCESS;
}

