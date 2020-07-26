#include <trogdor/actions/look.h>


namespace trogdor {

   bool LookAction::checkSyntax(const Command &command) {

      // we can only have one of either
      if (command.getDirectObject().length() > 0 &&
      command.getIndirectObject().length() > 0) {
         return false;
      }

      return true;
   }

   /***************************************************************************/

   void LookAction::execute(
      entity::Player *player,
      const Command &command,
      Game *game
   ) {

      std::string object = command.getDirectObject();

      if (object.length() == 0) {
         object = command.getIndirectObject();
      }

      if (auto location = player->getLocation().lock()) {

         std::shared_ptr<entity::Player> playerShared = player->getShared();

         if (object.length() == 0) {
            location->observe(playerShared, true, true);
         }

         else {

            entity::ThingList items;

            // consider matching inventory items, if there are any
            for (auto const &invObject: player->getInventoryObjectsByName(object)) {
                  items.push_front(invObject);
            };

            // also consider matching items in the room, if there are any
            for (auto const &roomThing: location->getThingsByName(object)) {
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
                  if (0 == object.compare(item->getName())) {
                     look(player, item);
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
               player->out("display") << "There is no " << object << " here!" << std::endl;
            }

            else if (items.size() > 1) {

               entity::Entity::clarifyEntity<entity::ThingList>(items, player);
               lookupThingByName[player] = playerShared;

               player->setInputInterceptor(std::make_unique<std::function<bool(std::string)>>(
                  [player, command](std::string input) -> bool {

                     Command cmd(player->getGame()->getVocabulary(), command.getVerb(), "", input);
                     return player->getGame()->executeAction(player, cmd);
                  }
               ));
            }

            else {
               look(player, *items.begin());
            }
         }
      }
   }
}
