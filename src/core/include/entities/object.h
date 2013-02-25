#ifndef OBJECT_H
#define OBJECT_H


#include "item.h"


namespace core { namespace entity {

   class Object: public Item {

      private:

      protected:

      public:

         /*
            Constructor for creating a new Object.  Requires reference to the
            containing Game object and a name.
         */
         inline Object(Game *g, string n): Item(g, n) {

            type = ENTITY_OBJECT;
         }
   };
}}


#endif

