#include <trogdor/entities/player.h>
#include <trogdor/actions/inventory.h>

namespace trogdor {


   bool InventoryAction::checkSyntax(const Command &command) {

      // A valid inventory command should only be one word, a verb
      if (command.getDirectObject().length() > 0 ||
      command.getIndirectObject().length() > 0) {
         return false;
      }

      return true;
   }

   /***************************************************************************/

   void InventoryAction::execute(
      entity::Player *player,
      const Command &command,
      Game *game
   ) {

      if (0 == player->getInventoryCount() && 0 == player->getResources().size()) {
         player->out("display") << "You don't have anything!" << std::endl;
      }

      else {

         // percentage of available space used
         double totalPercent = 0.0;

         player->out("display") << "Items in your inventory:" << std::endl << std::endl;

         // List resources
         for (auto const &allocation: player->getResources()) {
            if (auto resource = allocation.first.lock()) {
               player->out ("display") << resource->amountToString(allocation.second)
                  << ' ' << resource->titleToString(allocation.second) << std::endl;
            }
         }

         // List objects
         for (auto const &obj: player->getInventoryObjects()) {

            player->out("display") << obj->getProperty<std::string>(entity::Entity::TitleProperty);

            if (player->getInventoryMaxWeight() > 0) {

               if (obj->getProperty<int>(entity::Object::WeightProperty) > 0) {
                  double percent = 100 *
                     ((double)obj->getProperty<int>(entity::Object::WeightProperty) /
                     (double)player->getInventoryMaxWeight());
                  totalPercent += percent;
                  player->out("display") << " (" << percent << "%)";
               }

               else {
                  player->out("display") << " (weighs nothing)";
               }
            }

            player->out("display") << std::endl;
         };

         if (player->getInventoryMaxWeight() > 0) {
            player->out("display") << std::endl << "You are currently using "
               << totalPercent << "% of your inventory." << std::endl;
         }
      }
   }
}
