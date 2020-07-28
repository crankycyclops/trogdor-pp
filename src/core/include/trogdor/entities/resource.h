#ifndef RESOURCE_H
#define RESOURCE_H

#include <memory>
#include <unordered_map>

#include <trogdor/entities/entity.h>

class Tangible;

namespace trogdor::entity {


   // Represents abstract resources that can be divided up in some amount by
   // other entities in a game.
   class Resource: public Entity {

      private:

         // Keeps track of who holds the resource and how much
         std::unordered_map<
            std::weak_ptr<Tangible>,
            double,
            std::owner_less<std::weak_ptr<Tangible>>
         > holders;

      public:

         /*
            Constructor for creating a new Resource.

            Input:
               Reference to containing Game (Game *)
               Name (std::string)
         */
         Resource(Game *g, std::string n);

         /*
            Constructor for cloning one Resource into another.

            Input:
               Reference to entity to copy
               Name of copy (std::string)
         */
         Resource(const Resource &e, std::string n);

         /*
            Returns a smart pointer representing a raw Resource pointer. Be
            careful with this and only call it on Entities you know are managed
            by smart pointers. If, for example, you call this method on entities
            that are managed by Lua using new and delete, you're going to have a
            bad time.

            Input:
               (none)

            Output:
               std::shared_ptr<Entity>
         */
         inline std::shared_ptr<Resource> getShared() {

            return std::dynamic_pointer_cast<Resource>(Entity::getShared());
         }
   };
}


#endif
