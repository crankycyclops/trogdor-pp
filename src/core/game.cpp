using namespace std;

#include "include/game.h"

namespace core {

   Game::Game() {

      // default input and output streams
      terr = &cerr;
      tout = &cout;
      tin = &cin;

      inGame = false;
   }


   void Game::initialize(string gameXML) {

      // TODO: game initialization stuff goes here
      return;
   }


   void Game::start() {

      inGame = true;
   }
}
