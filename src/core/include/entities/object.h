#ifndef OBJECT_H
#define OBJECT_H


#include <memory>

#include "thing.h"
#include "../iostream/nullin.h"


namespace trogdor { namespace entity {


   class Being;

   class Object: public Thing {

      public:

         // This tag determines whether or not an Object is a weapon
         static const char *WeaponTag;

         // This tag determines whether or not an Object is untakeable
         static const char *UntakeableTag;

         // This tag determines whether or not an Object is undroppable
         static const char *UndroppableTag;

         static const bool DEFAULT_IS_WEAPON = false;
         static const int  DEFAULT_DAMAGE = 1;
         static const int DEFAULT_WEIGHT = 0;

      private:

         /*
            Call this whenever the "weapon" tag is set or removed for an Object.
            That way, if some runtime behavior changes an Object to or from a
            weapon, and it's owned by a Creature, we can tell that Creature it
            has to rebuild its weapons cache so it can properly choose a weapon
            during combat.

            Input:
               (none)

            Output:
               (none)
         */
         void updateOwnerWeaponCache();

      protected:

         Being *owner;     // a Being might own the object

         int weight;       // how much weight Object uses in a Being's inventory
         int damage;       // how much damage Object does if it's a weapon

      public:

         /*
            Constructor for creating a new Object.  Requires reference to the
            containing Game object and a name.
         */
         inline Object(Game *g, string n, std::unique_ptr<Trogout> o,
         std::unique_ptr<Trogout> e): Thing(g, n, std::move(o),
         std::make_unique<NullIn>(), std::move(e)), owner(nullptr),
         weight(DEFAULT_WEIGHT), damage(DEFAULT_DAMAGE) {

            if (DEFAULT_IS_WEAPON) {
               setTag(WeaponTag);
            }

            types.push_back(ENTITY_OBJECT);
            setClass("object");
         }

         /*
            Constructor for cloning an Object.  Requires a unique name.
         */
         inline Object(const Object &o, string n): Thing(o, n) {

            owner = o.owner;
            weight = o.weight;
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
            Wraps around Entity::setTag to see if any object-specific behavior
            needs to be invoked afterward.

            Input:
               Tag (string)

            Output:
               (none)
         */
         virtual void setTag(string tag);

         /*
            Wraps around Entity::removeTag to see if any object-specific behavior
            needs to be invoked afterward.

            Input:
               Tag (string)

            Output:
               (none)
         */
         virtual void removeTag(string tag);

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
