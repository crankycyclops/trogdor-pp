#include <iostream>
#include <cstdlib>

#include "include/game.h"
#include "include/actionmap.h"

using namespace std;

namespace core {


   Game::Game() {

      // default input and output streams
      trogerr = &cerr;
      trogout = &cout;
      trogin = &cin;

      inGame = false;
      actions = new ActionMap(this);
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

      Command *command = new Command();
      command->read(*trogin, *trogout);
*trogout << *command;
      if (!command->isInvalid()) {
         command->execute();
         // TODO: consider setting lastCommand
      }

      else {
         *trogout << "Sorry, I don't understand you.\n";
      }
   }
}

