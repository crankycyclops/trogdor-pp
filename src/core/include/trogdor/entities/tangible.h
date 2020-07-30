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
            Record the Entity's allocation of a specific resource.

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
         }

         /*
            Removes the Entity's allocation record for a specific resource.

            Input:
               Weak pointer to the resource
               The current allocated value
         */
         inline void removeResourceAllocation(const std::shared_ptr<Resource> &resource) {

            resourcesByName.erase(resource->getName());
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
