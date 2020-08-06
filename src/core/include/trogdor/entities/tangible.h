#ifndef TANGIBLE_H
#define TANGIBLE_H

#include <memory>
#include <unordered_map>

#include <trogdor/entities/resource.h>

namespace trogdor::entity {


   // All physically instantiable entities in the game inherit from this entity
   // type.
   class Tangible: public Entity {

      private:

         // maintains a list of all Beings that have glanced at but not fully
         // observed the Entity (cannot use unordered_set here because you can't
         // hash a weak_ptr.)
         std::set<
            std::weak_ptr<Being>,
            std::owner_less<std::weak_ptr<Being>>
         > glancedByMap;

         // maintains a list of all Beings that have observed the Entity (cannot
         // use unordered_set here because you can't hash a weak_ptr.)
         std::set<
            std::weak_ptr<Being>,
            std::owner_less<std::weak_ptr<Being>>
         > observedByMap;

         // Keeps track of which resources the entity holds and how much
         std::map<
            std::weak_ptr<Resource>,
            double,
            std::owner_less<std::weak_ptr<Resource>>
         > resources;

         // A name-based index into the resources the entity possesses
         std::unordered_map<
            std::string,
            std::weak_ptr<Resource>
         > resourcesByName;

         /*
            Record the Entity's allocation of a specific resource. Mutex locking
            is currently done by Resource::allocate().

            Input:
               Weak pointer to the resource
               The current allocated value
         */
         inline void recordResourceAllocation(
            const std::shared_ptr<Resource> &resource,
            double value
         ) {

            resources[resource] = value;
            resourcesByName[resource->getName()] = resource;
            resourcesByName[resource->getPluralName()] = resource;
         }

         /*
            Removes the Entity's allocation record for a specific resource.
            Mutex locking is currently done by Resource::free().

            Input:
               Weak pointer to the resource
               The current allocated value
         */
         inline void removeResourceAllocation(const std::shared_ptr<Resource> &resource) {

            resourcesByName.erase(resource->getName());
            resourcesByName.erase(resource->getPluralName());
            resources.erase(resource);
         }

         // Resource allocation should be handled solely by Resource::allocate()
         // and Resource::free()
         friend Resource::AllocateStatus Resource::allocate(
            const std::shared_ptr<Tangible> &entity,
            double amount
         );

         friend Resource::FreeStatus Resource::free(
            const std::shared_ptr<Tangible> &entity,
            double amount
         );

      public:

         /*
            Constructor for creating a new Tangible.

            Input:
               Reference to containing Game (Game *)
               Name (std::string)
               Pointer to output stream object (Trogout *)
               Pointer to error stream object (Trogerr *)
         */
         Tangible(Game *g, std::string n, std::unique_ptr<Trogout> o,
         std::unique_ptr<Trogerr> e);

         /*
            Constructor for cloning one Tangible into another.

            Input:
               Reference to entity to copy
               Name of copy (std::string)
         */
         Tangible(const Tangible &e, std::string n);

         /*
            Returns a smart pointer representing a raw Tangible pointer. Be
            careful with this and only call it on Entities you know are managed
            by smart pointers. If, for example, you call this method on entities
            that are managed by Lua using new and delete, you're going to have a
            bad time.

            Input:
               (none)

            Output:
               std::shared_ptr<Entity>
         */
         inline std::shared_ptr<Tangible> getShared() {

            return std::dynamic_pointer_cast<Tangible>(Entity::getShared());
         }

         /*
            Returns whether or not a given Being has observed the Tangible.

            Input:
               Being that may have observed the Tangible

            Output:
               bool
         */
         inline bool observedBy(const std::shared_ptr<Being> &b) {

            if (observedByMap.find(b) == observedByMap.end()) {
               return false;
            }

            return true;
         }

         /*
            Returns whether or not a given Being has glanced at the Tangible. If
            a Being has fully observed a Tangible, it stands to reason that the
            Being has also glanced at it, so both will be checked.

            Input:
               Being that may have glanced at the Tangible

            Output:
               bool
         */
         inline bool glancedBy(const std::shared_ptr<Being> &b) {

            if (observedByMap.find(b) == observedByMap.end() &&
            glancedByMap.find(b) == glancedByMap.end()) {
               return false;
            }

            return true;
         }

         /*
            Gives a Being the ability to observe an Entity.  If the Being is a
            Player, a description of what is seen (Entity's long description)
            will be printed to the output stream.

            Input:
               Being doing the observing
               Whether or not to trigger a before and after event
               Whether or not to always display the long description

            Output:
               (none)

            Events Triggered:
               beforeObserve
               afterObserve
         */
         virtual void observe(const std::shared_ptr<Being> &observer, bool triggerEvents = true,
         bool displayFull = false);

         /*
            Gives a Being the ability to partially observe an Entity without
            looking straight at it (what you might see during a cursory glance
            of a room, for example.)  If the Being is a Player, the Entity's
            short description will be printed to the output stream.

            Input:
               Being doing the observing
               Whether or not to trigger a before and after event

            Output:
               (none)

            Events Triggered:
               beforeGlance
               afterGlance
         */
         virtual void glance(const std::shared_ptr<Being> &observer, bool triggerEvents = true);

         /*
            Displays a Tangible.

            Input:
               Being doing the observing
               Whether or not to always display the full description

            Output:
               (none)
         */
         virtual void display(Being *observer, bool displayFull = false);

         /*
            Returns const reference to all resources held by the entity.

            Input:
               (none)

            Output:
               Const reference to resources
         */
         inline const auto &getResources() const {

            return resources;
         }

         /*
            Returns the allocation record for the specified resource if one
            exists or a std::pair with an expired weak_ptr and 0 if one doesn't.

            Input:
               (none)

            Output:
               std::pair<weak_ptr<Resource>, double> containing either a pointer
                  to the resource along with the amount the Entity currently
                  possesses or an expired pointer and 0 if no allocation is found.
         */
         inline std::pair<std::weak_ptr<Resource>, double> getResourceByName(
            std::string name
         ) const {

            if (
               resourcesByName.end() != resourcesByName.find(name) &&
               resources.end() != resources.find(resourcesByName.find(name)->second)
            ) {
               return *resources.find(resourcesByName.find(name)->second);
            }

            // If an allocation of th requested resource isn't found, we return
            // a std::pair with an expired weak_ptr and a value of 0.
            else {
               return {};
            }
         }
   };
}


#endif
