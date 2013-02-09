#include <iostream>
#include <cstdlib>

#include "include/game.h"

using namespace std;

namespace core {


   Game::Game() {

      // default input and output streams
      trogerr = &cerr;
      trogout = &cout;
      trogin = &cin;

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

      Command *command = Command::read(*trogin, *trogout);

      if (!command->isInvalid()) {
         command->execute();
      }

      else {
         *trogout << "Sorry, I don't understand you.\n";
      }
   }
}

