#ifndef ACTION_H
#define ACTION_H


#include "command.h"
#include "game.h"

using namespace std;

namespace trogdor { namespace core {


   /*
      This abstract class models a specific action in the game.  The verb in
      a user's command determines which action is executed.  All actions
      inherit from this class, which provides virtual functions that must be
      uniquely implemented for each action.
   */
   class Action {

      public:

         /*
            Checks the syntax of a command.  Returns true if the syntax is valid
            and false if it is not.  It's up to the specific action class to
            implement this method.

            Input:
               Command
            Output:
               bool
         */
         virtual bool checkSyntax(Command *command) = 0;

         /*
            Executes the action.  This mehod will be implemented by the specific
            action.

            Input:
               Player executing command
               Command
               Game in which command is made (Game *)
            Output:
               (none)
         */
         virtual void execute(Player *player, Command *command, Game *game) = 0;
   };
}}


#endif

