#ifndef READ_ACTION_H
#define READ_ACTION_H


#include <trogdor/actions/action.h>


namespace trogdor {

   // Allows players to read text written on objects inside the game.
   class ReadAction: public Action {

         /*
            See documentation in action.h.  A valid syntax for the Read action
            is to have a command with just a verb and a direct object. If the
            Thing referenced by the direct object has a value for
            getMeta("text"), that value will be read back. Otherwise, the player
            will be told that there is nothing to read.
         */
         virtual bool checkSyntax(const Command &command);

         virtual void execute(
            entity::Player *player,
            const Command &command,
            Game *game
         ); 
   };
}


#endif
