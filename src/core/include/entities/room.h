#ifndef ROOM_H
#define ROOM_H


#include <string>
#include <boost/unordered_map.hpp>

#include "../vocabulary.h"
#include "place.h"

using namespace std;
using namespace boost;


namespace core { namespace entity {

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
         inline Room(Game *g, string n, Trogout *o, Trogin *i): Place(g, n, o, i) {

            type = ENTITY_ROOM;
         }

         /*
            Returns a LuaTable object representing the Room.  Note that each
            child class should get the value of the parent class's version of
            this method and then fill that object in further with its own
            values.

            Input:
               (none)

            Output:
               LuaTable object
         */
         virtual LuaTable *getLuaTable() const;

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

            This is very hokey...

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
         */
         inline void setConnection(string direction, Room *connectTo) {

            stringstream s;

            if (isDirection(direction)) {
               connections[direction] = connectTo;
            }

            else {
               s << "error: attempt to connect to Room using invalid direction '"
                  << direction << "'";
               throw s.str();
            }
         }
   };
}}


#endif

