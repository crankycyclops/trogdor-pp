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
      if (0 == strToLower(command.getDirectObject()).compare("myself")) {

         static const char *suicideResponses[] = {
            "Done.",
            "If you insist...",
            "You know, they have hotlines for that sort of thing.",
            "Was life really so hard?",
            "I hope you left a note.",
            "You'll be missed."
         };

         static int arrSize = sizeof(suicideResponses) / sizeof (const char *);

         static std::random_device rd;
         static std::minstd_rand generator(rd());
         static std::uniform_int_distribution<unsigned> distribution(0, arrSize - 1);

         if (player->isAlive()) {

            player->out("display") << suicideResponses[distribution(generator)] << std::endl;
            player->die(true);
         }

         else {
            player->out("display") << "You're already dead." << std::endl;
         }

         return;
      }

      if (auto location = player->getLocation().lock()) {

         auto beings = location->getBeingsByName(command.getDirectObject());

         if (beings.begin() == beings.end()) {
            player->out("display") << "There is no " << command.getDirectObject()
               << " here!" << std::endl;
            return;
         }

         else if (beings.size() > 1) {

            entity::Entity::clarifyEntity<entity::BeingList>(beings, player);

            player->setInputInterceptor(std::make_unique<std::function<bool(std::string)>>(
               [player, command](std::string input) -> bool {

                  Command cmd(player->getGame()->getVocabulary(), command.getVerb(), input);
                  return player->getGame()->executeAction(player, cmd);
               }
            ));
         }

         else {

            entity::Being *defender = *beings.begin();
            std::string weaponName = command.getIndirectObject();
            entity::Object *weapon = 0;

            if (weaponName.length() > 0) {

               auto items = player->getInventoryObjectsByName(weaponName);

               if (items.begin() == items.end()) {
                  player->out("display") << "You don't have a " << weaponName << "!" << std::endl;
                  return;
               }

               else if (items.size() > 1) {

                  entity::Entity::clarifyEntity<entity::ObjectList>(items, player);

                  player->setInputInterceptor(std::make_unique<std::function<bool(std::string)>>(
                     [player, command](std::string input) -> bool {

                        Command cmd(
                           player->getGame()->getVocabulary(),
                           command.getVerb(),
                           command.getDirectObject(),
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
                     player->out("display") << "The " << weaponName << " isn't a weapon!" << std::endl;
                     return;
                  }
               }
            }

            player->attack(defender, weapon);
         }
      }
   }
}
