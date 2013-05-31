#ifndef OBJECT_H
#define OBJECT_H


#include "item.h"


namespace core { namespace entity {

   class Object: public Item {

      public:

         static const int DEFAULT_WEIGHT = 0;

      protected:

         int weight;       // how much weight Item uses in a Being's inventory

      public:

         /*
            Constructor for creating a new Object.  Requires reference to the
            containing Game object and a name.
         */
         inline Object(Game *g, Trogout *o, string n): Item(g, o, n) {

            type = ENTITY_OBJECT;
            weight = DEFAULT_WEIGHT;
         }

         /*
            Returns a LuaTable object representing the Object.  Note that each
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
            Returns the Item's weight.

            Input:
               (none)

            Output:
               The weight (int)
         */
         inline int getWeight() {return weight;}

         /*
            Sets the Item's weight (how much space it uses in a Being's
            inventory.)

            Input:
               New weight (int)

            Output:
               (none)
         */
         inline void setWeight(int w) {weight = w;}
   };
}}


#endif
