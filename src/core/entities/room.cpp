#include <trogdor/game.h>
#include <trogdor/entities/room.h>
#include <trogdor/exception/validationexception.h>


namespace trogdor::entity {


   void Room::setConnection(std::string direction, Room *connectTo) {

      if (game->getVocabulary().isDirection(direction)) {
         connections[direction] = connectTo;
      }

      else {
         throw ValidationException(
            std::string("error: attempt to connect to Room using invalid ")
            + "direction '" + direction + "'"
         );
      }
   }
}
