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

      if (0 == player->getInventoryObjects().size() && 0 == player->getResources().size()) {
         player->out("display") << "You don't have anything!" << std::endl;
      }

      // TODO: this will output incorrect results if any of the objects in gthe
      // player's inventory are weak_ptrs. Refactor to take this into account.
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

         int playerInvMaxWeight = player->getProperty<int>(entity::Being::InvMaxWeightProperty);

         // List objects
         for (auto const &objPtr: player->getInventoryObjects()) {

            if (const auto &obj = objPtr.second.lock()) {

               int objectWeight = obj->getProperty<int>(entity::Object::WeightProperty);

               player->out("display") << obj->getProperty<std::string>(entity::Entity::TitleProperty);

               if (playerInvMaxWeight > 0) {

                  if (objectWeight > 0) {
                     double percent = 100 * (
                        static_cast<double>(objectWeight) /
                        static_cast<double>(playerInvMaxWeight)
                     );
                     totalPercent += percent;
                     player->out("display") << " (" << percent << "%)";
                  }

                  else {
                     player->out("display") << " (weighs nothing)";
                  }
               }

               player->out("display") << std::endl;
            }
         };

         if (playerInvMaxWeight > 0) {
            player->out("display") << std::endl << "You are currently using "
               << totalPercent << "% of your inventory." << std::endl;
         }
      }
   }
}
