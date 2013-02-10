#include "include/game.h"
#include "include/actionmap.h"
#include "include/actions.h"

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


   void Game::initActions() {

      QuitAction *quit = new QuitAction;

      actions->setAction("quit", quit);
   }


   void Game::initialize(string gameXML) {

      initActions();
      return;
   }


   void Game::start() {

      inGame = true;
   }


   void Game::stop() {

      // TODO: cleanup?
      inGame = false;
   }


   void Game::processCommand() {

      Command *command = new Command();
      command->read(*trogin, *trogout);

      if (!command->isInvalid()) {

         Action *action = actions->getAction(command->getVerb());

         if (0 == action || !action->checkSyntax(command)) {
            *trogout << "Sorry, I don't understand you.\n";
         }

         else {
            pthread_mutex_lock(&resourceMutex);
            action->execute(command, this);
            pthread_mutex_unlock(&resourceMutex);
            // TODO: consider setting lastCommand
         }
      }

      else {
         *trogout << "Sorry, I don't understand you.\n";
      }
   }
}

