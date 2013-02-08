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
            Returns the status of the game.

            Input: (none)
            Output: boolean true if the game is running and false if it's not.
         */
         inline bool inProgress() {return inGame;}
   };
}

