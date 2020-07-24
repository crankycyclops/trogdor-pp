#ifndef ACTION_H
#define ACTION_H


#include <trogdor/command.h>
#include <trogdor/game.h>

namespace trogdor {

   namespace entity {

      // Forward declaration to prevent circular dependencies
      class Player;
   }


   /*
      This abstract class models a specific action in the game.  The verb in
      a user's command determines which action is executed.  All actions
      inherit from this class, which provides virtual functions that must be
      uniquely implemented for each action.
   */
   class Action {

      public:

         /*
            When a class has one or more virtual functions, it should also have
            a virtual destructor.
         */
         virtual ~Action() = 0;

         /*
            Checks the syntax of a command.  Returns true if the syntax is valid
            and false if it is not.  It's up to the specific action class to
            implement this method.

            Input:
               Command
            Output:
               bool
         */
         virtual bool checkSyntax(const Command &command) = 0;

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
         virtual void execute(
            entity::Player *player,
            const Command &command,
            Game *game
         ) = 0;
   };
}


#endif
