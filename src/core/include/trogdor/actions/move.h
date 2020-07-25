#ifndef MOVE_ACTION_H
#define MOVE_ACTION_H


#include <trogdor/actions/action.h>


namespace trogdor {

   // The Move action allows a player to move from one room to another.
   class MoveAction: public Action {

      public:

         /*
            See documentation in action.h.  A valid syntax for the Move action
            is to have a verb and either a direct object or indirect object
            (but not both) that matches a valid direction name, or a verb that
            matches a valid direction name.
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
