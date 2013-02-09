using namespace std;

#include <cstdlib>
#include "include/game.h"

namespace core {


   Game::Game() {

      // default input and output streams
      terr = &cerr;
      tout = &cout;
      tin = &cin;

      inGame = false;
      lastCommand = NULL;
   }


   Game::~Game() {

      if (NULL != lastCommand) {
         delete lastCommand;
      }
   }


   void Game::initialize(string gameXML) {

      // TODO: game initialization stuff goes here
      return;
   }


   void Game::start() {

      inGame = true;
   }


   void Game::processCommand() {
      // TODO
      exit(1);
   }
}

