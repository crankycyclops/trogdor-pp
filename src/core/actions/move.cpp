#include <trogdor/entities/player.h>
#include <trogdor/entities/room.h>

#include <trogdor/actions/move.h>

namespace trogdor {


   bool MoveAction::checkSyntax(const Command &command) {

      auto &vocab = command.getVocabulary();

      std::string verb = command.getVerb();
      std::string dobj = command.getDirectObject();
      std::string iobj = command.getIndirectObject();

      // A valid move command should not contain both a direct and indirect object
      if (dobj.length() > 0 && iobj.length() > 0) {
         return false;
      }

      // no direct or indirect object were given, so the direction, if valid,
      // must've been specified directly by the "verb"
      std::string dir = ((0 == dobj.length() && 0 == iobj.length()) ? verb :
         (dobj.length() > 0 ? dobj : iobj));
      return (vocab.isDirection(dir) || vocab.isDirectionSynonym(dir));
   }

   /***************************************************************************/

   void MoveAction::execute(
      entity::Player *player,
      const Command &command,
      Game *game
   ) {

      auto &vocab = command.getVocabulary();
      std::string direction = "";

      if (auto location = player->getLocation().lock()) {

         // direction is implied in the verb
         if (vocab.isDirection(command.getVerb()) || vocab.isDirectionSynonym(command.getVerb())) {
            direction = vocab.getDirection(command.getVerb());
         }

         // direction was supplied as the direct or indirect object of another
         // verb like "move" or "go"
         if (0 == direction.length()) {
            direction = vocab.getDirection(command.getDirectObject().length() > 0 ?
               command.getDirectObject() : command.getIndirectObject());
         }

         // only Rooms have connections to eachother
         if (entity::ENTITY_ROOM != location->getType()) {
            player->out("display") << "You can't go that way." << std::endl;
            // TODO: fire can't go that way event?
            return;
         }

         auto next = (std::static_pointer_cast<entity::Room>(location))->getConnection(direction);

         if (nullptr == next) {
            player->out("display") << "You can't go that way." << std::endl;
            // TODO: fire can't go that way event?
            return;
         }

         std::string enterMessage = next->getMessage("enter" + direction);
         std::string goMessage = location->getMessage("go" + direction);

         if (goMessage.length() > 0) {
            player->out("display") << goMessage << std::endl << std::endl;
         }

         if (enterMessage.length() > 0) {
            player->out("display") << enterMessage << std::endl << std::endl;
         }

         player->gotoLocation(next->getShared());
      }
   }
}
