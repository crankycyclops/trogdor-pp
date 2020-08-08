#ifndef LOOK_ACTION_H
#define LOOK_ACTION_H


#include <memory>
#include <unordered_map>

#include <trogdor/actions/action.h>
#include <trogdor/entities/player.h>


namespace trogdor {

   // Allows a player to look at things
   class LookAction: public Action {

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

         // Observe the specified object
         inline void look(entity::Player *player, entity::Thing *thing) {

            thing->observe(player->getShared(), true, true);
         }

         /********************************************************************/

         inline void lookAtResource(
            entity::Player *player,
            entity::Tangible *depositor,
            std::string resourceName,
            std::optional<double> resourceQty
         ) {

            auto allocation = depositor->getResourceByName(resourceName);

            if (auto resource = allocation.first.lock()) {

               double amount = resolveResourceAmount(
                  resource.get(),
                  resourceName,
                  allocation.second,
                  resourceQty
               );

               operateOnResource(resource.get(), depositor, player, amount, [&] {
                  resource->observe(player->getShared(), amount);
               });
            }
         }

      public:

         /*
            See documentation in action.h.  A valid syntax for the Look action
            is to have a command with just a verb or one of a direct or indirect
            object.  Just the look verb with no indirect object will observe the
            player's current location.  Looking at a direct object will result
            in the player observing that object (if it exists in the user's
            current Location OR inventory.)
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
