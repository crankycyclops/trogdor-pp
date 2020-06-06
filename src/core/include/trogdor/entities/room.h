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

         // connections to other rooms (implemented as a hash table so that we
         // can later trivially add additional directions)
         std::unordered_map<std::string, Room *> connections;

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
            Returns room connected by the specified direction.  Returns 0 if
            the connection does not exist.

            Input:
               Direction (std::tring)

            Output:
               Room *
         */
         inline Room *getConnection(std::string direction) const {

            if (connections.find(direction) == connections.end()) {
               return 0;
            }

            return connections.find(direction)->second;
         }

         /*
            Returns the number of connections.

            Input:
               (none)

            Output:
               Number of connections (unsigned int)
         */
         inline unsigned int getNumConnections() const {return connections.size();}

         /*
            Returns a connected room by numeric index (in so far as we iterate
            through all values until we've counted index iterations and then
            return the result.)  This, in conjunction with getNumConnections(),
            is used primarily for the random selection of a connection.

            If index > getNumConnections() - 1, then the first connection will
            be returned (arbitrary decision.)

            Returns a null pointer if there are no connections.

            This is very hokey, but ultimately allows us to get a random
            connection, which is useful specifically for Creatures that wander.

            Input:
               index (unsigned int)

            Output:
               connected Room (Room *)
         */
         inline Room *getConnectionByIndex(unsigned int i) const {

            if (0 == getNumConnections()) {
               return 0;
            }

            else if (i > getNumConnections() - 1) {
               return connections.begin()->second;
            }

            else {

               std::unordered_map<std::string, Room *>::const_iterator c = connections.begin();

               for (; i > 0; i--) {
                  c++;
               }

               return c->second;
            }
         }

         /*
            Connects Room to another Room at the specified direction.

            Input:
               Direction where the connection is made (string)
               Room direction connects to (Room *)

            Output:
               (none)
         */
         void setConnection(std::string direction, Room *connectTo);
   };
}


#endif
