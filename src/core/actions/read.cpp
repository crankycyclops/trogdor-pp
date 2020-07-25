#include <trogdor/entities/player.h>
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

      if (auto location = player->getLocation().lock()) {

         if (thingName.length() == 0) {
            location->observe(player->getShared(), true, true);
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

            if (0 == items.size()) {
               player->out("display") << "There is no " << thingName << " here!" << std::endl;
               return;
            }

            else if (items.size() > 1) {

               entity::Entity::clarifyEntity<entity::ThingList>(items, player);

               player->setInputInterceptor(std::make_unique<std::function<bool(std::string)>>(
                  [player, command](std::string input) -> bool {

                     Command cmd(player->getGame()->getVocabulary(), command.getVerb(), input);
                     return player->getGame()->executeAction(player, cmd);
                  }
               ));
            }

            else {

               entity::Thing *thing = *items.begin();
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
         }
      }
   }
}
