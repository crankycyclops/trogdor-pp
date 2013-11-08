#ifndef OBJECT_H
#define OBJECT_H


#include "item.h"


namespace core { namespace entity {

   class Object: public Item {

      public:

         static const int DEFAULT_WEIGHT = 0;

      protected:

         int weight;       // how much weight Object uses in a Being's inventory

      public:

         /*
            Constructor for creating a new Object.  Requires reference to the
            containing Game object and a name.
         */
         inline Object(Game *g, string n, Trogout *o, Trogin *i, Trogout *e):
         Item(g, n, o, i, e) {

            types.push_back(ENTITY_OBJECT);
            weight = DEFAULT_WEIGHT;

            setClass("object");
         }

         /*
            Constructor for cloning an Object.  Requires a unique name.
         */
         inline Object(const Object &o, string n): Item(o, n) {

            weight = o.weight;
         }

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

         /*
            Extends Thing::addAlias(), and ensures that if the Object is owned,
            that the owner's inventory's index by name is updated.

            Input:
               New alias (string)

            Output:
               (none)
         */
         virtual void addAlias(string alias);
   };
}}


#endif
