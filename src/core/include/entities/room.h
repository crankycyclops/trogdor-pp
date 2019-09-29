#ifndef ROOM_H
#define ROOM_H


#include <string>
#include <memory>
#include <unordered_map>

#include "../vocabulary.h"
#include "../exception/validationexception.h"

#include "place.h"


using namespace std;

namespace trogdor { namespace entity {


   class Room: public Place {

      protected:

         // connections to other rooms (implemented as a hash table so that we
         // can later trivially add additional directions)
         unordered_map<string, Room *> connections;

      public:

         /*
            Constructor for creating a new Room.  Requires reference to the
            containing Game object and a name.
         */
         inline Room(Game *g, string n, std::unique_ptr<Trogout> o,
         std::unique_ptr<Trogout> e): Place(g, n, std::move(o), std::move(e)) {

            types.push_back(ENTITY_ROOM);
            setClass("room");
         }

         /*
            Constructor for cloning an existing Room.  Requires a unique name.
         */
         inline Room(const Room &r, string n): Place(r, n) {

            // TODO: trying to decide if this makes sense?
            connections = r.connections;
         }

         /*
            Returns room connected by the specified direction.  Returns 0 if
            the connection does not exist.

            Input:
               Direction (string)

            Output:
               Room *
         */
         inline Room *getConnection(string direction) const {

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
               Number of connections (size_t)
         */
         inline size_t getNumConnections() const {return connections.size();}

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
               index (int)

            Output:
               connected Room (Room *)
         */
         inline Room *getConnectionByIndex(int i) const {

            if (0 == getNumConnections()) {
               return 0;
            }

            else if (i > getNumConnections() - 1) {
               return connections.begin()->second;
            }

            else {

               unordered_map<string, Room *>::const_iterator c = connections.begin();

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
         void setConnection(string direction, Room *connectTo);
   };
}}


#endif

