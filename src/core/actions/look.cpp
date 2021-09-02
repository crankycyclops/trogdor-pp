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
      std::optional<double> amount = command.getDirectObjectQty();

      if (object.length() == 0) {
         object = command.getIndirectObject();
         amount = command.getIndirectObjectQty();
      }

      if (0 == object.compare(player->getName()) || 0 == strToLower(object).compare("myself")) {
         player->out() << "You can't look at yourself!" << std::endl;
         return;
      }

      if (auto location = player->getLocation().lock()) {

         std::shared_ptr<entity::Player> playerShared = player->getShared();

         // Player isn't looking at anything in particular, so observe room
         if (object.length() == 0) {
            location->observe(playerShared, true, true);
         }

         // Player is looking at a resource they possess
         else if (!player->getResourceByName(object).first.expired()) {
            lookAtResource(player, player, object, amount);
         }

         // Player is looking at a resource in the room
         else if (!location->getResourceByName(object).first.expired()) {
            lookAtResource(player, location.get(), object, amount);
         }

         else {

            std::list<entity::Thing *> items;

            // consider matching inventory items, if there are any
            for (auto const &invObjectPtr: player->getInventoryObjectsByName(object)) {
                  if (auto const &invObject = invObjectPtr.lock()) {
                     items.push_front(invObject.get());
                  }
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
