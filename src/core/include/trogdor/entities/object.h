#ifndef OBJECT_H
#define OBJECT_H


#include <memory>

#include <trogdor/entities/thing.h>
#include <trogdor/iostream/nullin.h>


namespace trogdor::entity {


   class Being;

   class Object: public Thing {

      public:

         // Tag is set if the Object is a weapon
         static const char *WeaponTag;

         // Tag is set if the Object is untakeable
         static const char *UntakeableTag;

         // Tag is set if the Object is undroppable
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
         Object(Game *g, std::string n, std::unique_ptr<Trogout> o,
         std::unique_ptr<Trogerr> e);

         /*
            Constructor for cloning an Object.  Requires a unique name.
         */
         Object(const Object &o, std::string n);

         /*
            Returns a smart pointer representing a raw Object pointer. Be careful
            with this and only call it on Entities you know are managed by smart
            pointers. If, for example, you call this method on entities that are
            managed by Lua using new and delete, you're going to have a bad time.

            Input:
               (none)

            Output:
               std::shared_ptr<Object>
         */
         inline std::shared_ptr<Object> getShared() {

            return std::dynamic_pointer_cast<Object>(Entity::getShared());
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
               Tag (std::tring)

            Output:
               (none)
         */
         virtual void setTag(std::string tag);

         /*
            Wraps around Entity::removeTag to see if any object-specific behavior
            needs to be invoked afterward.

            Input:
               Tag (std::string)

            Output:
               (none)
         */
         virtual void removeTag(std::string tag);

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
               New alias (std::string)

            Output:
               (none)
         */
         virtual void addAlias(std::string alias);
   };
}


#endif
