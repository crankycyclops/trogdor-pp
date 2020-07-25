#include <trogdor/entities/player.h>
#include <trogdor/actions/drop.h>

#include <trogdor/exception/beingexception.h>

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

      auto invItems = player->getInventoryObjectsByName(command.getDirectObject());

      if (invItems.begin() == invItems.end()) {
         player->out("display") << "You don't have a " << command.getDirectObject()
            << "!" << std::endl;
         return;
      }

      else if (invItems.size() > 1) {

         entity::Entity::clarifyEntity<entity::ObjectList>(invItems, player);

         player->setInputInterceptor(std::make_unique<std::function<bool(std::string)>>(
            [player, command](std::string input) -> bool {

               Command cmd(player->getGame()->getVocabulary(), command.getVerb(), input);
               return player->getGame()->executeAction(player, cmd);
            }
         ));
      }

      else {

         entity::Object *object = *invItems.begin();

         try {

            player->drop(object->getShared());

            std::string message = object->getMessage("drop");
            if (message.length() > 0) {
               player->out("display") << message << std::endl;
            }

            else {
               player->out("display") << "You drop the " << object->getName()
                  << "." << std::endl;
            }
         }

         catch (const entity::BeingException &e) {

            switch (e.getErrorCode()) {

               case entity::BeingException::DROP_UNDROPPABLE:
                  // TODO: add message for this (named undroppable)
                  player->out("display") << "You can't drop that!" << std::endl;
                  break;

               default:
                  player->err() << "Unknown error dropping object.  This is a "
                     << "bug." << std::endl;
                  break;
            }
         }
      }
   }
}
