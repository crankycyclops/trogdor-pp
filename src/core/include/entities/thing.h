#ifndef THING_H
#define THING_H



#include <vector>
#include "entity.h"
#include "place.h"


namespace core { namespace entity {

   class Thing: public Entity {

      protected:

         Place          *location;  // where the thing is located
         vector<string>  synonyms;  // list of synonyms for the thing

      public:

         /*
            Constructor for creating a new Thing.  Requires reference to the
            containing Game object and a name.
         */
         inline Thing(Game *g, string n): Entity(g, n) {}
   };
}}


#endif

