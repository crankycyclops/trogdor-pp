#ifndef GAME_H
#define GAME_H


#include <iostream>
#include <string>

#include "command.h"

using namespace std;

namespace core {

   class ActionMap; // resolves circular dependency ActionMap <-> Game

   /*
      Each Game object represents a self contained game and contains all
      game-related data.  A standard lifecycle of a game object -- assuming it
      exists in main() -- might look something like:

      Game currentGame = new Game();

      currentGame.initialize();

      currentGame.start();
      while (currentGame.inProgress()) {
         ...
         read/execute a command;
         ...
      }

      delete currentGame;
      return EXIT_SUCCESS;
   */
   class Game {

      private:

         bool       inGame;        // whether or not a game is in progress
         ActionMap  *actions;      // maps verbs to actions
         Command    *lastCommand;  // the last executed command

         /*
            Called by initialize().  This initializes game actions and maps
            them to verbs.

            Input: (none)
            Output: (none)
         */
         void initActions();

      public:

         ostream *trogerr;  /* error output stream */
         ostream *trogout;  /* console output stream */
         istream *trogin;   /* input stream */

         /*
            Constructor for the Game class.
         */
         Game();

         /*
            Destructor for the Game class.
         */
         ~Game();

         /*
            Sets the game's error output stream.

            Input: new output stream (ostream)
            Output: (none)
         */
         inline void setTrogerr(ostream *newerr) {trogerr = newerr;}

         /*
            Sets the game's standard output stream.

            Input: new output stream (ostream)
            Output: (none)
         */
         inline void setTrogout(ostream *newout) {trogout = newout;}

         /*
            Sets the game's input stream.

            Input: new input stream (istream)
            Output: (none)
         */
         inline void setTrogin(istream *newin) {trogin = newin;}

         /*
            Initializes the game, including the event handler, timer and all game
            entities.

            Input: Game definition XML file (default is game.xml)
            Output: (none)
         */
         void initialize(string gameXML = "game.xml");

         /*
            Puts the game into a running state.

            Input: (none)
            Output: (none)
         */
         void start();

         /*
            Returns the status of the game.

            Input: (none)
            Output: boolean true if the game is running and false if it's not.
         */
         inline bool inProgress() const {return inGame;}

         /*
            Reads a command from the user and executes it.

            Input: (none)
            Output: (none)
         */
         void processCommand();
   };
}


#endif

