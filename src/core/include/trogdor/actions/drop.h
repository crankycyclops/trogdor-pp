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

         /********************************************************************/

         // If the resource is ephemeral, dropResource() will call this method
         // to make the player's resource allocation disappear.
         inline void freeResource(
            entity::Player *player,
            std::shared_ptr<entity::Resource> &resource,
            double amount
         ) {

            switch(resource->free(player->getShared(), amount)) {

               case entity::Resource::ALLOCATE_OR_FREE_SUCCESS:

                  player->out("display") << "You drop "
                     << resource->amountToString(amount) << ' '
                     << resource->titleToString(amount) << '.' << std::endl;
                  break;

               // If the action was aborted due to an event handler, we'll let
               // that event send appropriate feedback to the player
               case entity::Resource::ALLOCATE_OR_FREE_ABORT:

                  break;

               // An error occurred :(
               default:

                  if (resource->getMessage("resourceCantDrop").length()) {
                     player->out("display") << resource->getMessage("resourceCantDrop")
                        << std::endl;
                  }

                  else {
                     player->out("display") << "You can't drop that!" << std::endl;
                  }

                  break;
            }
         }

         /********************************************************************/

         // If the resource is not ephemeral, dropResource() will call this
         // method to drop the resource into the room where the player currently
         // resides.
         inline void transferResourceToRoom(
            entity::Player *player,
            std::shared_ptr<entity::Place> &location,
            std::shared_ptr<entity::Resource> &resource,
            double amount
         ) {

            switch (resource->transfer(player->getShared(), location, amount)) {

               // Yay!
               case entity::Resource::ALLOCATE_OR_FREE_SUCCESS:

                  player->out("display") << "You drop "
                     << resource->amountToString(amount) << ' '
                     << resource->titleToString(amount) << '.' << std::endl;
                  break;

               // The room has alrady reached max capacity for this resource
               // and can't take anymore
               case entity::Resource::ALLOCATE_MAX_PER_DEPOSITOR_EXCEEDED:

                  player->out("display") << "This place can only hold "
                     << resource->amountToString(*resource->getMaxAmountPerDepositor())
                     << ' ' << resource->titleToString(*resource->getMaxAmountPerDepositor())
                     << '.' << std::endl;

                  break;

               // If the action was aborted due to an event handler, we'll let
               // that event send appropriate feedback to the player
               case entity::Resource::ALLOCATE_OR_FREE_ABORT:

                  break;

               // An error occurred :(
               default:

                  if (resource->getMessage("resourceCantDrop").length()) {
                     player->out("display") << resource->getMessage("resourceCantDrop")
                        << std::endl;
                  }

                  else {
                     player->out("display") << "You can't drop that!" << std::endl;
                  }

                  break;
            }
         }

         /********************************************************************/

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

         /********************************************************************/

         inline void dropResource(
            entity::Player *player,
            std::string resourceName,
            std::optional<double> resourceQty
         ) {

            auto allocation = player->getResourceByName(resourceName);

            if (auto resource = allocation.first.lock()) {

               double amount = resolveResourceAmount(
                  resource.get(),
                  resourceName,
                  allocation.second,
                  resourceQty
               );

               operateOnResource(resource.get(), player, player, amount, [&] {

                  if (resource->isTagSet(entity::Resource::ephemeralTag)) {
                     freeResource(player, resource, amount);
                  }

                  else {

                     if (auto location = player->getLocation().lock()) {
                        transferResourceToRoom(player, location, resource, amount);
                     }
                  }
               });
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
