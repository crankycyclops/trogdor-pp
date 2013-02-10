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
   };
}}


#endif

