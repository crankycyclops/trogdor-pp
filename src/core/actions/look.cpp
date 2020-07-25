#include <trogdor/entities/player.h>
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

         if (object.length() == 0) {
            location->observe(player->getShared(), true, true);
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

            if (0 == items.size()) {
               player->out("display") << "There is no " << object << " here!" << std::endl;
               return;
            }

            else if (items.size() > 1) {

               entity::Entity::clarifyEntity<entity::ThingList>(items, player);

               player->setInputInterceptor(std::make_unique<std::function<bool(std::string)>>(
                  [player, command](std::string input) -> bool {

                     Command cmd(player->getGame()->getVocabulary(), command.getVerb(), "", input);
                     return player->getGame()->executeAction(player, cmd);
                  }
               ));
            }

            else {
               (*items.begin())->observe(player->getShared(), true, true);
            }
         }
      }
   }
}
