#include <trogdor/actions/drop.h>

namespace trogdor {


   bool DropAction::checkSyntax(const Command &command) {

      if (command.getIndirectObject().length() > 0 ||
      command.getDirectObject().length() == 0) {
         return false;
      }

      return true;
   }

   /***************************************************************************/

   // TODO: consider custom messages
   void DropAction::execute(
      entity::Player *player,
      const Command &command,
      Game *game
   ) {

      std::shared_ptr<entity::Player> playerShared = player->getShared();
      auto invItems = player->getInventoryObjectsByName(command.getDirectObject());

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
         for (auto &item: invItems) {
            if (0 == command.getDirectObject().compare(item->getName())) {
               drop(player, item);
               break;
            }
         }

         lookupThingByName.erase(player);
      }

      else if (invItems.begin() == invItems.end()) {
         player->out("display") << "You don't have a " << command.getDirectObject()
            << "!" << std::endl;
         return;
      }

      else if (invItems.size() > 1) {

         entity::Entity::clarifyEntity<entity::ObjectList>(invItems, player);
         lookupThingByName[player] = playerShared;

         player->setInputInterceptor(std::make_unique<std::function<bool(std::string)>>(
            [player, command](std::string input) -> bool {

               Command cmd(player->getGame()->getVocabulary(), command.getVerb(), input);
               return player->getGame()->executeAction(player, cmd);
            }
         ));
      }

      else {
         drop(player, *invItems.begin());
      }
   }
}
