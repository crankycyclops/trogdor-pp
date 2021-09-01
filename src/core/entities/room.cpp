#include <trogdor/game.h>
#include <trogdor/entities/room.h>
#include <trogdor/entities/being.h>
#include <trogdor/exception/validationexception.h>


namespace trogdor::entity {


   Room::Room(
      Game *g,
      std::string n,
      std::unique_ptr<Trogout> o,
      std::unique_ptr<Trogerr> e
   ): Place(g, n, std::move(o), std::move(e)) {

      types.push_back(ENTITY_ROOM);
      setClass("room");
   }

   /**************************************************************************/

   Room::Room(const Room &r, std::string n): Place(r, n) {

      // TODO: trying to decide if this makes sense?
      connections = r.connections;
   }

   /***************************************************************************/

   Room::Room(
      Game *g,
      const serial::Serializable &data,
      std::unique_ptr<Trogout> o,
      std::unique_ptr<Trogerr> e
   ): Place(g, data, std::move(o), std::move(e)) {

      g->addCallback("afterDeserialize",
      std::make_shared<Entity::EntityCallback>([&](std::any) -> bool {

         std::shared_ptr<serial::Serializable> serializedConnections =
            std::get<std::shared_ptr<serial::Serializable>>(*data.get("connections"));

         for (const auto &connection: serializedConnections->getAll()) {

            if (const std::shared_ptr<Room> &roomPtr =
            game->getRoom(std::get<std::string>((connection.second)))) {
               connections[connection.first] = roomPtr;
            }
         }

         return true;
      }));

      types.push_back(ENTITY_ROOM);
   }

   /****************************************************************************/

   void Room::displayConnections(Being *observer, bool displayFull) {

      if (!observedBy(observer->getShared()) || displayFull) {

         std::vector<std::string> toRemove;

         for (const auto &description: connectionDescriptions) {
            if (getConnection(description.first)) {
               observer->out("display") << std::endl << description.second << std::endl;
            } else {
               toRemove.push_back(description.first);
            }
         }

         // If we encountered any stale connection descriptions, remove them.
         for (const auto &badConnection: toRemove) {
            connectionDescriptions[badConnection].erase();
         }
      }
   }

   /****************************************************************************/

   void Room::display(Being *observer, bool displayFull) {

      displayPlace(observer, displayFull);
      displayConnections(observer, displayFull);
      displayResources(observer);
      displayThings(observer);
   }

   /***************************************************************************/

   std::shared_ptr<serial::Serializable> Room::serialize() {

      std::shared_ptr<serial::Serializable> data = Place::serialize();
      std::shared_ptr<serial::Serializable> serializedConnections = std::make_shared<serial::Serializable>();

      for (const auto &connection: connections) {
         if (const auto &connectedRoom = connection.second.lock()) {
            serializedConnections->set(connection.first, connectedRoom->getName());
         }
      }

      data->set("connections", serializedConnections);
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

   void Room::setConnection(
      std::string direction,
      const std::shared_ptr<Room> &connectTo,
      std::optional<std::string> description
   ) {

      if (game->getVocabulary().isDirection(direction)) {

         mutex.lock();
         connections[direction] = connectTo;

         if (description) {
            connectionDescriptions[direction] = *description;
         }

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
