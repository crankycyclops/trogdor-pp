#ifndef OBJECT_H
#define OBJECT_H


#include <memory>

#include <trogdor/entities/thing.h>


namespace trogdor::entity {


   class Being;

   class Object: public Thing {

      protected:

         // A Being might own the object
         std::weak_ptr<Being> owner;

      public:

         // How much the object weighs, which affects how many
         // and which items can be carried in a Being's inventory
         static constexpr const char *WeightProperty = "weight";

         // How much damage Object does if it's a weapon
         static constexpr const char *DamageProperty = "damage";

         // Tag is set if the Object is a weapon
         static constexpr const char *WeaponTag = "weapon";

         // Tag is set if the Object is untakeable
         static constexpr const char *UntakeableTag = "untakeable";

         // Tag is set if the Object is undroppable
         static constexpr const char *UndroppableTag = "undroppable";

         static constexpr bool DEFAULT_IS_WEAPON = false;
         static constexpr int  DEFAULT_DAMAGE = 1;
         static constexpr int DEFAULT_WEIGHT = 0;

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
            This constructor deserializes an Object.

            Input:
               Raw deserialized data (const Serializable &)
         */
         Object(const serial::Serializable &data);

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
            Returns the owner of the Object (returns an invalid weak_ptr if
            there is no owner.)

            Input:
               (none)

            Output:
               std::weak_ptr<Being>
         */
         inline std::weak_ptr<Being> getOwner() const {return owner;}

         /*
            Sets the owner.

            Input:
               Being that should now own the object

            Output:
               (none)
         */
         inline void setOwner(std::weak_ptr<Being> being) {

            mutex.lock();
            owner = being;
            mutex.unlock();
         }

         /*
            Serializes the Being.
            Input:
               (none)
            Output:
               An object containing easily serializable data (Serializable)
         */
         virtual serial::Serializable serialize();

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
