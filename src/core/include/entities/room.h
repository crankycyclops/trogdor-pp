#ifndef ROOM_H
#define ROOM_H


#include "place.h"


namespace core { namespace entity {

   class Room: public Place {

      protected:

         // connections to other rooms
         Room *north;
         Room *south;
         Room *east;
         Room *west;
         Room *up;
         Room *down;
         Room *in;
         Room *out;

      public:

         /*
            Constructor for creating a new Room.  Requires reference to the
            containing Game object and a name.
         */
         inline Room(Game *g, string n): Place(g, n) {}
   };
}}


#endif

