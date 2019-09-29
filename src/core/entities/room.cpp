#include "../include/game.h"
#include "../include/entities/room.h"
#include "../include/exception/validationexception.h"


using namespace std;

namespace trogdor { namespace entity {


   void Room::setConnection(string direction, Room *connectTo) {

      if (game->getVocabulary().isDirection(direction)) {
         connections[direction] = connectTo;
      }

      else {
         throw ValidationException(
            string("error: attempt to connect to Room using invalid ")
            + "direction '" + direction + "'"
         );
      }
   }
}}

