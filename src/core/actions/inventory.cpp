#include <queue>

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

      std::queue<std::string> invLines;
      int playerInvMaxWeight = player->getProperty<int>(entity::Being::InvMaxWeightProperty);

      // percentage of available space used
      double totalPercent = 0.0;

      // List resources
      for (auto const &allocation: player->getResources()) {
         if (auto resource = allocation.first.lock()) {
            invLines.push(resource->amountToString(allocation.second) + ' ' + resource->titleToString(allocation.second));
         }
      }

      // List objects
      for (auto const &objPtr: player->getInventoryObjects()) {

         if (const auto &obj = objPtr.second.lock()) {

            std::string line = obj->getProperty<std::string>(entity::Entity::TitleProperty);
            int objectWeight = obj->getProperty<int>(entity::Object::WeightProperty);

            if (playerInvMaxWeight > 0) {

               if (objectWeight > 0) {
                  double percent = 100 * (
                     static_cast<double>(objectWeight) /
                     static_cast<double>(playerInvMaxWeight)
                  );
                  totalPercent += percent;
                  line += " (" + percent + "%)";
               }

               else {
                  line += " (weighs nothing)";
               }
            }

            invLines.push(line);
         }
      }

      if (!invLines.size()) {
         player->out("display") << "You don't have anything!" << std::endl;
      }

      else {

         player->out("display") << "Items in your inventory:" << std::endl << std::endl;

         while (invLines.size()) {
            player->out("display") << invLines.front() << std::endl;
            invLines.pop();
         }

         if (playerInvMaxWeight > 0) {
            player->out("display") << std::endl << "You are currently using "
               << totalPercent << "% of your inventory." << std::endl;
         }
      }
   }
}
