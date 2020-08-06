#include <trogdor/actions/read.h>

namespace trogdor {


   bool ReadAction::checkSyntax(const Command &command) {

      // we must have a direct object and only a direct object
      if (!command.getDirectObject().length() || command.getIndirectObject().length()) {
         return false;
      }

      return true;
   }

   void ReadAction::execute(
      entity::Player *player,
      const Command &command,
      Game *game
   ) {

      std::string thingName = command.getDirectObject();
      std::optional<double> thingAmount;

      if (thingName.length()) {
         thingAmount = command.getDirectObjectQty();
      } else {
         thingName = command.getIndirectObject();
         thingAmount = command.getIndirectObjectQty();
      }

      if (auto location = player->getLocation().lock()) {

         std::shared_ptr<entity::Player> playerShared = player->getShared();

         if (!playerShared->getResourceByName(thingName).first.expired()) {
            readResource(game, player, player, thingName, thingAmount);
         }

         else if (!location->getResourceByName(thingName).first.expired()) {
            readResource(game, player, location.get(), thingName, thingAmount);
         }

         else {

            entity::ThingList items;

            // consider matching inventory items, if there are any
            for (auto const &invObject: player->getInventoryObjectsByName(thingName)) {
               items.push_front(invObject);
            };

            // also consider matching items in the room, if there are any
            for (auto const &roomThing: location->getThingsByName(thingName)) {
               items.push_front(roomThing);
            };

            // We're calling this action a second time after asking the user to
            // supply a unique name out of a list of items with the same alias,
            // so now we need to lookup the item by name instead of by alias.
            if (
               lookupThingByName.end() != lookupThingByName.find(player) &&
               !lookupThingByName[player].expired() &&
               lookupThingByName[player].lock() == playerShared
            ) {

               bool itemFound = false;

               // This list should be pretty small, so I think it's okay to just
               // iterate through them until we find the right one.
               for (auto &item: items) {
                  if (0 == command.getDirectObject().compare(item->getName())) {
                     read(game, player, item);
                     itemFound = true;
                     break;
                  }
               }

               lookupThingByName.erase(player);

               if (!itemFound) {
                  player->out("display") << "There is no " << command.getDirectObject()
                     << " here!" << std::endl;
               }
            }

            else if (0 == items.size()) {
               player->out("display") << "There is no " << thingName << " here!" << std::endl;
               return;
            }

            else if (items.size() > 1) {

               entity::Entity::clarifyEntity<entity::ThingList>(items, player);
               lookupThingByName[player] = playerShared;

               player->setInputInterceptor(std::make_unique<std::function<bool(std::string)>>(
                  [player, command](std::string input) -> bool {

                     Command cmd(player->getGame()->getVocabulary(), command.getVerb(), input);
                     return player->getGame()->executeAction(player, cmd);
                  }
               ));
            }

            else {
               read(game, player, *items.begin());
            }
         }
      }
   }
}
