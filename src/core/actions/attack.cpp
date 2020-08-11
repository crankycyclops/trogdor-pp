#include <random>

#include <trogdor/entities/player.h>
#include <trogdor/actions/attack.h>

namespace trogdor {


   bool AttackAction::checkSyntax(const Command &command) {

      if (command.getDirectObject().length() == 0) {
         return false;
      }

      return true;
   }

   /***************************************************************************/

   void AttackAction::execute(
      entity::Player *player,
      const Command &command,
      Game *game
   ) {

      // The player is self-terminating
      // TODO: you can say "attack myself with fish" even if you don't have a
      // fish in your inventory. While technically not a big deal in the case of
      // self-termination, I should probably fix this.
      if (
         0 == strToLower(command.getDirectObject()).compare("myself") ||
         0 == command.getDirectObject().compare(player->getName())
      ) {
         commitSuicide(player);
         return;
      }

      if (auto location = player->getLocation().lock()) {

         std::shared_ptr<entity::Player> playerShared = player->getShared();

         // The Being the player will be attacking and the weapon the player
         // will be using (if this remains nullptr after a call to selectWeapon(),
         // the player will be attacking without one.)
         entity::Being *defender = nullptr;
         entity::Object *weapon = nullptr;

         auto beings = location->getBeingsByName(command.getDirectObject());

         if (
            lookupDefenderByName.end() != lookupDefenderByName.find(player) &&
            !lookupDefenderByName[player].expired() &&
            lookupDefenderByName[player].lock() == playerShared
         ) {

            for (auto &being: beings) {
               if (0 == command.getDirectObject().compare(being->getName())) {
                  defender = being;
                  break;
               }
            }

            lookupDefenderByName.erase(player);

            if (!defender) {
               player->out("display") << "There is no " << command.getDirectObject()
                  << " here!" << std::endl;
               return;
            }
         }

         else if (beings.begin() == beings.end()) {

            if (
               location->getThingsByName(command.getDirectObject()).size() ||
               !location->getResourceByName(command.getDirectObject()).first.expired()
            ) {
               player->out("display") << "You can't attack that!" << std::endl;
            }

            else {
               player->out("display") << "There is no " << command.getDirectObject()
                  << " here!" << std::endl;
            }

            return;
         }

         else if (beings.size() > 1) {

            entity::Entity::clarifyEntity<entity::BeingList>(beings, player);
            lookupDefenderByName[player] = playerShared;

            player->setInputInterceptor(std::make_unique<std::function<bool(std::string)>>(
               [player, command](std::string input) -> bool {

                  Command cmd(player->getGame()->getVocabulary(), command.getVerb(), input);
                  return player->getGame()->executeAction(player, cmd);
               }
            ));

            return;
         }

         else {
            defender = *beings.begin();
         }

         if (
            lookupWeaponByName.end() != lookupWeaponByName.find(player) &&
            !lookupWeaponByName[player].expired() &&
            lookupWeaponByName[player].lock() == playerShared
         ) {

            for (auto &item: player->getInventoryObjectsByName(command.getIndirectObject())) {
               if (0 == command.getDirectObject().compare(item->getName())) {
                  weapon = item;
                  break;
               }
            }

            lookupWeaponByName.erase(player);

            if (!weapon) {
               player->out("display") << "You don't have a " << command.getDirectObject()
                  << '!' << std::endl;
               return;
            }
         }

         else if (command.getIndirectObject().length() > 0) {

            auto items = player->getInventoryObjectsByName(command.getIndirectObject());

            if (items.begin() == items.end()) {

               if (!player->getResourceByName(command.getIndirectObject()).first.expired()) {
                  player->out("display") << "You can't attack with that!" << std::endl;
               }

               else {
                  player->out("display") << "You don't have a "
                     << command.getIndirectObject() << '!' << std::endl;
               }

               return;
            }

            else if (items.size() > 1) {

               entity::Entity::clarifyEntity<entity::ObjectList>(items, player);
               auto defenderShared = defender->getShared();

               // By supplying the chosen defender's name in the command and
               // forcing us to look that up by name, we avoid a potential bug
               // in the case where we have to clarify both the defender and the
               // weapon.
               lookupDefenderByName[player] = playerShared;
               lookupWeaponByName[player] = playerShared;

               player->setInputInterceptor(std::make_unique<std::function<bool(std::string)>>(
                  [player, defenderShared, command](std::string input) -> bool {

                     Command cmd(
                        player->getGame()->getVocabulary(),
                        command.getVerb(),
                        defenderShared->getName(),
                        input,
                        "with"
                     );

                     return player->getGame()->executeAction(player, cmd);
                  }
               ));

               return;
            }

            else {

               weapon = *items.begin();

               // TODO: this check should be made inside Being (we'd have an
               // exception to catch)
               if (!weapon->isTagSet(entity::Object::WeaponTag)) {
                  player->out("display") << "The " << command.getIndirectObject() << " isn't a weapon!" << std::endl;
                  return;
               }
            }
         }

         player->attack(defender, weapon);
      }
   }
}
