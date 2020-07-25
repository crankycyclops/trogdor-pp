#ifndef TAKE_ACTION_H
#define TAKE_ACTION_H


#include <trogdor/actions/action.h>


namespace trogdor {

   // Allows players to take objects in the game and add them to their
   // inventories.
   class TakeAction: public Action {

      public:

         /*
            See documentation in action.h.  A valid syntax for the Take action
            is to have a verb + direct object.  The result is for the Object
            to be taken into the Being's inventory, if possible.
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
