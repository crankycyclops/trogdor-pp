#ifndef INVENTORY_ACTION_H
#define INVENTORY_ACTION_H


#include <trogdor/actions/action.h>


namespace trogdor {


   // The Inventory action lists the items in a Player's inventory.
   class InventoryAction: public Action {

      public:

         /*
            See documentation in action.h.  A valid syntax for the Inventory
            action is to have a command with just a verb and no direct object or
            indirect object.
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
