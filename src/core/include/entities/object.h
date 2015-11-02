#ifndef OBJECT_H
#define OBJECT_H


#include "thing.h"


namespace core { namespace entity {

   class Being;

   class Object: public Thing {

      public:

         static const bool DEFAULT_IS_WEAPON = false;
         static const int  DEFAULT_DAMAGE = 1;
         static const int DEFAULT_WEIGHT = 0;

         // by default, a Being can take and drop the Object
         static const bool DEFAULT_TAKEABLE = true;
         static const bool DEFAULT_DROPPABLE = true;

      protected:

         Being *owner;     // a Being might own the object

         int weight;       // how much weight Object uses in a Being's inventory
         int damage;       // how much damage Object does if it's a weapon

         bool takeable;    // whether or not a Being can take the Object
         bool droppable;   // whether or not a Being can drop the Object
         bool weapon;      // whether or not Object is a weapon

      public:

         /*
            Constructor for creating a new Object.  Requires reference to the
            containing Game object and a name.
         */
         inline Object(Game *g, string n, Trogout *o, Trogin *i, Trogout *e):
         Thing(g, n, o, i, e),
         owner(0), weight(DEFAULT_WEIGHT), takeable(DEFAULT_TAKEABLE),
         droppable(DEFAULT_DROPPABLE), weapon(DEFAULT_IS_WEAPON),
         damage(DEFAULT_DAMAGE) {

            types.push_back(ENTITY_OBJECT);
            setClass("object");
         }

         /*
            Constructor for cloning an Object.  Requires a unique name.
         */
         inline Object(const Object &o, string n): Thing(o, n) {

            owner = o.owner;
            weight = o.weight;
            takeable = o.takeable;
            droppable = o.droppable;
            weapon = o.weapon;
            damage = o.damage;
         }

         /*
            Returns the owner of the Object (0 if there is no owner.)

            Input:
               (none)

            Output:
               Being *
         */
         inline Being *getOwner() const {return owner;};


         /*
            Returns the Object's weight.

            Input:
               (none)

            Output:
               The weight (int)
         */
         inline int getWeight() {return weight;}

         /*
            Returns whether or not the Object can be taken.

            Input:
               (none)

            Output:
               bool
         */
         inline bool getTakeable() {return takeable;}

         /*
            Returns whether or not the Object can be dropped.

            Input:
               (none)

            Output:
               bool
         */
         inline bool getDroppable() {return droppable;}

         /*
            Returns whether or not Object is a weapon.

            Input:
               (none)

            Output:
               Bool
         */
         inline bool isWeapon() {return weapon;}

         /*
            Returns damage Object does if it's a weapon, measured in hit points.

            Input:
               (none)

            Output:
               Damage in hit points (int)
         */
         inline int getDamage() {return damage;}

         /*
            Sets the owner.

            Input:
               Being that should now own the object

            Output:
               (none)
         */
         inline void setOwner(Being *being) {owner = being;}

         /*
            Sets the Object's weight (how much space it uses in a Being's
            inventory.)

            Input:
               New weight (int)

            Output:
               (none)
         */
         inline void setWeight(int w) {weight = w;}


         /*
            Sets whether or not a Being can take the object.

            Input:
               bool

            Output:
               (none)
         */
         inline void setTakeable(bool t) {takeable = t;}

         /*
            Sets whether or not a Being can drop the object.

            Input:
               bool

            Output:
               (none)
         */
         inline void setDroppable(bool d) {droppable = d;}

         /*
            Sets whether or not Object is a weapon.

            Input:
               Bool

            Output:
               (none)
         */
         void setIsWeapon(bool w);

         /*
            Sets amount of damage Object does if it's a weapon (measured in hit
            points.)

            Input:
               Damage in hit points (int)

            Output:
               (none)
         */
         inline void setDamage(int d) {damage = d;}

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
