#ifndef TAKE_ACTION_H
#define TAKE_ACTION_H


#include <unordered_map>

#include <trogdor/entities/player.h>
#include <trogdor/actions/action.h>

#include <trogdor/exception/beingexception.h>


namespace trogdor {

   // Allows players to take objects in the game and add them to their
   // inventories.
   class TakeAction: public Action {

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

         // Takes the object and adds it to the player's inventory
         inline void take(entity::Player *player, entity::Thing *thing) {

            if (entity::ENTITY_OBJECT != thing->getType()) {
               player->out("display") << "You can't take that!" << std::endl;
            }

            else {

               try {

                  player->take(static_cast<entity::Object *>(thing)->getShared());

                  std::string message = thing->getMessage("take");
                  if (message.length() > 0) {
                     player->out("display") << message << std::endl;
                  }

                  else {
                     player->out("display") << "You take the " << thing->getName()
                        << "." << std::endl;
                  }
               }

               catch (const entity::BeingException &e) {

                  // TODO: consider custom messages
                  switch (e.getErrorCode()) {

                     case entity::BeingException::TAKE_TOO_HEAVY:
                        player->out("display") << thing->getName()
                           << " is too heavy.  Try dropping something first."
                           << std::endl;
                        break;

                     case entity::BeingException::TAKE_UNTAKEABLE:
                        player->out("display") << "You can't take that!" << std::endl;
                        break;

                     default:
                        player->err() << "Unknown error taking object.  "
                           << "This is a bug." << std::endl;
                        break;
                  }
               }
            }
         }

         inline void takeResource(
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

                  std::string titleStr = resource->areIntegerAllocationsRequired() &&
                     1 == std::lround(amount) ? resource->getTitle() :
                     resource->getPluralTitle();

                  // The player might not have an allocation, so I don't know in
                  // advance if I can do this. I would have set a variable, but
                  // I can't do so under a case.
                  auto getPlayerAmountStr = [&]() -> std::string {

                     return resource->amountToString(
                        player->getResources().find(resource)->second
                     );
                  };

                  auto getAmountAllowedStr = [&]() -> std::string {

                     return resource->amountToString(
                        *resource->getMaxAmountPerDepositor()
                     );
                  };

                  if (auto location = player->getLocation().lock()) {

                     switch (resource->transfer(location, player->getShared(), amount)) {

                        // Transfers must be made in integer amounts
                        case entity::Resource::FREE_INT_REQUIRED:
                        case entity::Resource::ALLOCATE_INT_REQUIRED:

                           player->out("display") << "Please specify a whole number of "
                              << resource->getPluralTitle() << '.' << std::endl;
                           break;

                        // The transfer would result in the player possessing
                        // more of the resource than they're allowed to have
                        case entity::Resource::ALLOCATE_MAX_PER_DEPOSITOR_EXCEEDED:

                           player->out("display") << "You already have "
                              << getPlayerAmountStr() << ' ' << resource->getPluralTitle()
                              << " and are only allowed to possess "
                              << getAmountAllowedStr() << '.' << std::endl;
                           break;

                        // We can't transfer an amount less than or equal to 0
                        case entity::Resource::FREE_NEGATIVE_VALUE:
                        case entity::Resource::ALLOCATE_ZERO_OR_NEGATIVE_AMOUNT:

                           player->out("display") << "Please specify an amount greater than zero."
                              << std::endl;
                           break;

                        // The original holder of the resource doesn't have the
                        // requested amount
                        case entity::Resource::FREE_EXCEEDS_ALLOCATION:

                           player->out("display") << "You can only take "
                              << resource->amountToString(allocation.second)
                              << ' ' << resource->getPluralTitle() << '.' << std::endl;
                           break;

                        // Success!
                        case entity::Resource::ALLOCATE_OR_FREE_SUCCESS:

                           // Notify every entity in the room that the resource has
                           // been taken EXCEPT the one who's doing the taking
                           for (auto const &thing: location->getThings()) {
                              if (thing.get() != player) {
                                 thing->out("notifications") << player->getTitle()
                                    << " takes " << resource->amountToString(amount) << ' '
                                    << titleStr << '.' << std::endl;
                              }
                           };

                           player->out("display") << "You take "
                              << resource->amountToString(amount) << ' '
                              << titleStr << '.' << std::endl;

                           break;

                        // It's possible we'll get here if ALLOCATE_ABORT is
                        // returned due to cancellation by the event handler. In
                        // such a case, we'll let the specific event trigger that
                        // canceled the allocation handle the explanitory output.
                        default:
                           break;
                     }
                  }
               });
            }
         }

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
