#include <trogdor/game.h>
#include <trogdor/entities/room.h>
#include <trogdor/exception/validationexception.h>


namespace trogdor::entity {


   Room::Room(Game *g, std::string n, std::unique_ptr<Trogout> o,
   std::unique_ptr<Trogerr> e): Place(g, n, std::move(o), std::move(e)) {

      types.push_back(ENTITY_ROOM);
      setClass("room");
   }

   /**************************************************************************/

   Room::Room(const Room &r, std::string n): Place(r, n) {

      // TODO: trying to decide if this makes sense?
      connections = r.connections;
   }

   /**************************************************************************/

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
