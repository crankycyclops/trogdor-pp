#ifndef READ_ACTION_H
#define READ_ACTION_H


#include <unordered_map>

#include <trogdor/game.h>
#include <trogdor/entities/player.h>
#include <trogdor/entities/resource.h>
#include <trogdor/actions/action.h>


namespace trogdor {

   // Allows players to read text written on objects inside the game.
   class ReadAction: public Action {

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

         // Complete the read operation if there's text that can be read.
         inline void read(Game *game, entity::Player *player, entity::Thing *thing) {

            std::string text = thing->getMeta("text");

            if (!game->event({
               "beforeRead",
               {player->getEventListener(), thing->getEventListener()},
               {player, thing}
            })) {
               return;
            }

            if (text.length()) {
               player->out("display") << text << std::endl;
            }

            else {
               player->out("display") << "There's nothing to read." << std::endl;
            }

            game->event({
               "afterRead",
               {player->getEventListener(), thing->getEventListener()},
               {player, thing}
            });
         }

         /********************************************************************/

         // Does the same as read() except it reads a resource with an allocated
         // amount. Takes as input the name of the resource as the player typed
         // it, the amount the player typed in (might not contain a value), and
         // the amount currently allocated to the room or the player and uses
         // those values to determine how much of the resource we're "reading."
         inline void readResource(
            Game *game,
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

                  if (!game->event({
                     "beforeRead",
                     {player->getEventListener(), resource->getEventListener()},
                     {player, resource.get(), amount}
                  })) {
                     return;
                  }

                  std::string text = resource->getMeta("text");

                  if (!text.length()) {
                     player->out("display") << "There's nothing to read." << std::endl;
                  }

                  else {
                     player->out("display") << text << std::endl;
                  }

                  game->event({
                     "afterRead",
                     {player->getEventListener(), resource->getEventListener()},
                     {player, resource.get(), amount}
                  });
               });
            }
         }

      public:

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
