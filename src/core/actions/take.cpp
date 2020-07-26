#include <trogdor/entities/player.h>
#include <trogdor/actions/take.h>

#include <trogdor/exception/beingexception.h>

namespace trogdor {

   bool TakeAction::checkSyntax(const Command &command) {

      if (command.getIndirectObject().length() > 0 ||
      command.getDirectObject().length() == 0) {
         return false;
      }

      return true;
   }

   void TakeAction::execute(
      entity::Player *player,
      const Command &command,
      Game *game
   ) {

      if (auto location = player->getLocation().lock()) {

         std::shared_ptr<entity::Player> playerShared = player->getShared();
         auto roomItems = location->getThingsByName(command.getDirectObject());

         // We're calling this action a second time after asking the user to
         // supply a unique name out of a list of items with the same alias,
         // so now we need to lookup the item by name instead of by alias.
         if (
            lookupThingByName.end() != lookupThingByName.find(player) &&
            !lookupThingByName[player].expired() &&
            lookupThingByName[player].lock() == playerShared
         ) {

            // This list should be pretty small, so I think it's okay to just
            // iterate through them until we find the right one.
            for (auto &item: roomItems) {
               if (0 == command.getDirectObject().compare(item->getName())) {
                  take(player, item);
                  break;
               }
            }

            lookupThingByName.erase(player);
         }

         else if (0 == roomItems.size()) {
            player->out("display") << "There is no " << command.getDirectObject()
               << " here!" << std::endl;
            return;
         }

         else if (roomItems.size() > 1) {

            entity::Entity::clarifyEntity<entity::ThingList>(roomItems, player);
            lookupThingByName[player] = playerShared;

            player->setInputInterceptor(std::make_unique<std::function<bool(std::string)>>(
               [player, command](std::string input) -> bool {

                  Command cmd(player->getGame()->getVocabulary(), command.getVerb(), input);
                  return player->getGame()->executeAction(player, cmd);
               }
            ));
         }

         else {
            take(player, *roomItems.begin());
         }
      }
   }
}
