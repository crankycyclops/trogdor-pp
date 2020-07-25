#ifndef DROP_ACTION_H
#define DROP_ACTION_H


#include <trogdor/actions/action.h>


namespace trogdor {

   // Allows a player to drop an item from their inventory, leaving it behind
   // in whatever room they currently occupy.
   class DropAction: public Action {

         /*
            See documentation in action.h.  A valid syntax for the Drop action
            is to have a verb + direct object.  The result is for the Object
            to be removed from the Being's inventory and dropped into the Being's
            current location, if possible.
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
