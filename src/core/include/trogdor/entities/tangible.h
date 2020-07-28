#ifndef TANGIBLE_H
#define TANGIBLE_H

#include <memory>
#include <unordered_map>

#include <trogdor/entities/entity.h>

class Resource;

namespace trogdor::entity {


   // All physically instantiable entities in the game inherit from this entity
   // type.
   class Tangible: public Entity {

      private:

         // Keeps track of which resources the entity holds and how much
         std::unordered_map<
            std::weak_ptr<Resource>,
            double,
            std::owner_less<std::weak_ptr<Resource>>
         > resources;

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
   };
}


#endif
