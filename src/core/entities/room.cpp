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

   /***************************************************************************/

   Room::Room(const serial::Serializable &data): Place(data) {

      // TODO
   }

   /***************************************************************************/

   std::shared_ptr<serial::Serializable> Room::serialize() {

      std::shared_ptr<serial::Serializable> data = Place::serialize();

      // TODO
      return data;
   }

   /**************************************************************************/

   std::shared_ptr<Room> Room::getConnectionByIndex(size_t i) {

      std::string direction;
      std::weak_ptr<Room> connection;

      if (0 == getNumConnections()) {
         return std::shared_ptr<Room>();
      }

      else if (i > getNumConnections() - 1) {
         direction = connections.begin()->first;
         connection = connections.begin()->second;
      }

      else {

         auto c = connections.begin();

         for (; i > 0; i--) {
            c++;
         }

         direction = c->first;
         connection = c->second;
      }

      std::shared_ptr<Room> retVal = connection.lock();

      if (!retVal) {
         mutex.lock();
         connections.erase(direction);
         mutex.unlock();
      }

      return retVal;
   }

   /**************************************************************************/

   void Room::setConnection(std::string direction, const std::shared_ptr<Room> &connectTo) {

      if (game->getVocabulary().isDirection(direction)) {
         mutex.lock();
         connections[direction] = connectTo;
         mutex.unlock();
      }

      else {
         throw ValidationException(
            std::string("error: attempt to connect to Room using invalid ")
            + "direction '" + direction + "'"
         );
      }
   }
}
