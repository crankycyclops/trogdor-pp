#include <iostream>
#include <string>

using namespace std;

namespace core {

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

         bool inGame;

      public:

         ostream *terr;  /* error output stream */
         ostream *tout;  /* console output stream */
         istream *tin;   /* input stream */

         /*
            Constructor for the Game class.
         */
         Game();

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
            Sets the game's error output stream.

            Input: new output stream (ostream)
            Output: (none)
         */
         inline void setTerr(ostream *newerr) {terr = newerr;}

         /*
            Sets the game's standard output stream.

            Input: new output stream (ostream)
            Output: (none)
         */
         inline void setTout(ostream *newout) {tout = newout;}

         /*
            Sets the game's input stream.

            Input: new input stream (istream)
            Output: (none)
         */
         inline void setTin(istream *newin) {tin = newin;}

         /*
            Returns the status of the game.

            Input: (none)
            Output: boolean true if the game is running and false if it's not.
         */
         inline bool inProgress() {return inGame;}
   };
}
