#ifndef ROOM_H
#define ROOM_H


#include <string>
#include <memory>
#include <unordered_map>

#include <trogdor/vocabulary.h>
#include <trogdor/exception/validationexception.h>

#include <trogdor/entities/place.h>


namespace trogdor::entity {


   class Room: public Place {

      protected:

         // maps directions to connected rooms
         std::unordered_map<std::string, std::weak_ptr<Room>> connections;

      public:

         /*
            Constructor for creating a new Room.  Requires reference to the
            containing Game object and a name.
         */
         Room(Game *g, std::string n, std::unique_ptr<Trogout> o,
         std::unique_ptr<Trogerr> e);

         /*
            Constructor for cloning an existing Room.  Requires a unique name.
         */
         Room(const Room &r, std::string n);

         /*
            This constructor deserializes a Room.

            Input:
               Raw deserialized data (const Serializable &)
         */
         Room(const serial::Serializable &data);

         /*
            Returns a smart pointer representing a raw Room pointer. Be careful
            with this and only call it on Entities you know are managed by smart
            pointers. If, for example, you call this method on entities that are
            managed by Lua using new and delete, you're going to have a bad time.

            Input:
               (none)

            Output:
               std::shared_ptr<Room>
         */
         inline std::shared_ptr<Room> getShared() {

            return std::dynamic_pointer_cast<Room>(Entity::getShared());
         }

         /*
            Returns room connected by the specified direction.  Returns an empty
            shared pointer if the connection doesn't exist or if the connection
            is to a room that no longer exists. If the connection is for a room
            that doesn't exist, it will be removed before an empty shared_ptr is
            returned.

            Input:
               Direction (std::tring)

            Output:
               std::shared_ptr<Room>
         */
         inline std::shared_ptr<Room> getConnection(std::string direction) {

            if (connections.find(direction) == connections.end()) {
               return std::shared_ptr<Room>();
            }

            std::shared_ptr<Room> connection = connections[direction].lock();

            if (!connection) {
               mutex.lock();
               connections.erase(direction);
               mutex.unlock();
            }

            return connection;
         }

         /*
            Returns the number of connections.

            Input:
               (none)

            Output:
               Number of connections (size_t)
         */
         inline size_t getNumConnections() const {return connections.size();}

         /*
            Serializes the Room.

            Input:
               (none)

            Output:
               An object containing easily serializable data (Serializable)
         */
         virtual serial::Serializable serialize();

         /*
            Returns a connected room by numeric index (in so far as we iterate
            through all values until we've counted index iterations and then
            return the result.)  This, in conjunction with getNumConnections(),
            is used primarily for the random selection of a connection.

            If index > getNumConnections() - 1, then the first connection will
            be returned (arbitrary decision.)

            Returns an empty shared_ptr if there are no connections. If the
            connection refers to a pointer that's no longer valid, it will be
            removed.

            This is very hokey, but ultimately allows us to get a random
            connection, which is useful specifically for Creatures that wander.

            Input:
               index (size_t)

            Output:
               std::shared_ptr<Room>
         */
         std::shared_ptr<Room> getConnectionByIndex(size_t i);

         /*
            Connects Room to another Room at the specified direction.

            Input:
               Direction where the connection is made (string)
               Room direction connects to (const std::shared_ptr<Room> &)

            Output:
               (none)
         */
         void setConnection(std::string direction, const std::shared_ptr<Room> &connectTo);
   };
}


#endif
