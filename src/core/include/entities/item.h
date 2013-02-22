#ifndef ITEM_H
#define ITEM_H


#include "thing.h"
#include "being.h"


namespace core { namespace entity {

   class Item: public Thing {

      protected:

         Being *owner;

      public:

         /*
            Constructor for creating a new Item.  Requires reference to the
            containing Game object and a name.
         */
         inline Item(Game *g, string n): Thing(g, n) {}
   };
}}


#endif

