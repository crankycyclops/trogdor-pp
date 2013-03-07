#ifndef ACTIONS_H
#define ACTIONS_H


#include "action.h"

using namespace std;

namespace core {


   /*
      The Quit action stops the game, ordinarily returning control to the client.
   */
   class QuitAction: public Action {

      public:

         /*
            See documentation in action.h.  A valid syntax for the Quit action
            is to have a command with just a verb and no direct object or
            indirect object.
         */
         virtual bool checkSyntax(Command *command);

         virtual void execute(Player *player, Command *command, Game *game);
   };

/******************************************************************************/

   class LookAction: public Action {

         /*
            See documentation in action.h.  A valid syntax for the Look action
            is to have a command with just a verb or one of a direct or indirect
            object.  Just the look verb with no indirect object will observe the
            player's current location.  Looking at a direct object will result
            in the player observing that object (if it exists in the user's
            current Location OR inventory.)
         */
         virtual bool checkSyntax(Command *command);

         virtual void execute(Player *player, Command *command, Game *game); 
   };

/******************************************************************************/

   /*
      The Move action allows a player to move from one room to another.
   */
   class MoveAction: public Action {

      public:

         /*
            See documentation in action.h.  A valid syntax for the Move action
            is to have a verb and either a direct object or indirect object
            (but not both) that matches a valid direction name, or a verb that
            matches a valid direction name.
         */
         virtual bool checkSyntax(Command *command);

         virtual void execute(Player *player, Command *command, Game *game);
   }; 
}


#endif

