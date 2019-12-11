#include "../include/game.h"
#include "../include/entities/room.h"
#include "../include/exception/validationexception.h"


namespace trogdor { namespace entity {


   void Room::setConnection(string direction, Room *connectTo) {

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
}}

