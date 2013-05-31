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
         inline Room(Game *g, Trogout *o, string n): Place(g, o, n) {

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

