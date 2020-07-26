#ifndef DROP_ACTION_H
#define DROP_ACTION_H


#include <unordered_map>
#include <trogdor/entities/player.h>
#include <trogdor/actions/action.h>

#include <trogdor/exception/beingexception.h>


namespace trogdor {

   // Allows a player to drop an item from their inventory, leaving it behind
   // in whatever room they currently occupy.
   class DropAction: public Action {

      private:

         // If a player is executing this action from an input interceptor that
         // attempts to pick one Thing to look at from a list of available
         // options with the same alias, make sure we lookup the item by its
         // name rather than its alias to avoid infinite calls to clarifyEntity()
         // in the case where the desired object's name matches an alias with
         // more than one Thing.
         std::unordered_map<
            entity::Player *,
            std::weak_ptr<entity::Player>
         > lookupThingByName;

         // Drops the object from the player's inventory
         inline void drop(entity::Player *player, entity::Object *object) {

            try {

               player->drop(object->getShared());

               std::string message = object->getMessage("drop");
               if (message.length() > 0) {
                  player->out("display") << message << std::endl;
               }

               else {
                  player->out("display") << "You drop the " << object->getName()
                     << "." << std::endl;
               }
            }

            catch (const entity::BeingException &e) {

               switch (e.getErrorCode()) {

                  case entity::BeingException::DROP_UNDROPPABLE:
                     // TODO: add message for this (named undroppable)
                     player->out("display") << "You can't drop that!" << std::endl;
                     break;

                  default:
                     player->err() << "Unknown error dropping object.  This is a "
                        << "bug." << std::endl;
                     break;
               }
            }
         }

      public:

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
