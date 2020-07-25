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

         auto roomItems = location->getThingsByName(command.getDirectObject());

         if (0 == roomItems.size()) {
            player->out("display") << "There is no " << command.getDirectObject()
               << " here!" << std::endl;
            return;
         }

         else if (roomItems.size() > 1) {

            entity::Entity::clarifyEntity<entity::ThingList>(roomItems, player);

            player->setInputInterceptor(std::make_unique<std::function<bool(std::string)>>(
               [player, command](std::string input) -> bool {

                  Command cmd(player->getGame()->getVocabulary(), command.getVerb(), input);
                  return player->getGame()->executeAction(player, cmd);
               }
            ));
         }

         else {

            entity::Thing *thing = *roomItems.begin();

            if (entity::ENTITY_OBJECT != thing->getType()) {
               player->out("display") << "You can't take that!" << std::endl;
            }

            else {

               try {

                  player->take(static_cast<entity::Object *>(thing)->getShared());

                  std::string message = thing->getMessage("take");
                  if (message.length() > 0) {
                     player->out("display") << message << std::endl;
                  }

                  else {
                     player->out("display") << "You take the " << thing->getName()
                        << "." << std::endl;
                  }
               }

               catch (const entity::BeingException &e) {

                  // TODO: consider custom messages
                  switch (e.getErrorCode()) {

                     case entity::BeingException::TAKE_TOO_HEAVY:
                        player->out("display") << command.getDirectObject()
                           << " is too heavy.  Try dropping something first."
                           << std::endl;
                        break;

                     case entity::BeingException::TAKE_UNTAKEABLE:
                        player->out("display") << "You can't take that!" << std::endl;
                        break;

                     default:
                        player->err() << "Unknown error taking object.  "
                           << "This is a bug." << std::endl;
                        break;
                  }
               }
            }
         }
      }
   }
}
