#ifndef ENTITYMAP_H
#define ENTITYMAP_H


#include <string>
#include <memory>
#include <unordered_map>

#include <trogdor/entities/entity.h>
#include <trogdor/entities/place.h>
#include <trogdor/entities/thing.h>
#include <trogdor/entities/room.h>
#include <trogdor/entities/being.h>
#include <trogdor/entities/player.h>
#include <trogdor/entities/creature.h>
#include <trogdor/entities/object.h>


namespace trogdor { namespace entity {


   /*
      EntityMap wraps around the unordered_map type and maps names to entities.

      There are derived classes for each entity type, which go by the name of
      "<Type>Map."  These do nothing except wrap around the parent getter and
      cast the result to the more specific type.

      The entity map is utilized by the Parser when constructing game objects
      from the game.xml file, and also in the Game object to maintain a list of
      all the things that are in the game.
   */
   class EntityMap {

      // What we use internally to store string -> Entity mappings
      typedef std::unordered_map<std::string, std::shared_ptr<Entity>> entityUnorderedMap;

      private:

         // Hash table mapping names to entities
         entityUnorderedMap entities;

      protected:

         /*
            Returns the shared_ptr associated with the given name instead of
            the raw pointer. This is to grant read-only access to the get()
            method in classes for derived types (Thing, Place, etc.)

            Input: Entity name (std::string)
            Output: Entity's shared_ptr (or nullptr if it doesn't exist)
         */
         inline const std::shared_ptr<Entity> &getSharedPtr(std::string name) {

            if (isEntitySet(name)) {
               return entities.find(name)->second;
            } else {
               throw false;
            }
         }

      public:

         /*
            Destructor for EntityMap.
         */
         inline ~EntityMap() {clear();}

         /*
            Returns the number of entities currently stored in the map.

            Input: (none)
            Output: (none)
         */
         inline int size() const {return entities.size();}

         /* Returns true if the entity with the given name is set and false if
            it's not.

            Input: Entity name (std::string)
            Output: true if it's set and false if it's not
         */
         inline bool isEntitySet(std::string name) const {

            return entities.find(name) == entities.end() ? false : true;
         }

         /*
            Gets an entity by name.  If it doesn't exist, NULL will be
            returned.

            Input: Entity name (std::string)
            Output: Pointer to entity (Entity *)
         */
         inline Entity *get(const std::string name) {

            return isEntitySet(name) ? entities.find(name)->second.get() : nullptr;
         }

         /*
            Sets an entity.  If the entity already exists, it will be
            overwritten.

            Input: Entity name (std::string), Pointer to entity
            Output: (none)
         */
         inline void set(std::string name, std::shared_ptr<Entity> entity) {entities[name] = entity;}

         /*
            Removes an entity from the hash table.

            Input: Name of Entity
            Output: (none)
         */
         inline void erase(std::string name) {entities.erase(name);}

         /*
            Removes all entities from the EntityMap.  The entities themselves
            are not destroyed!

            Input: (none)
            Output: (none)
         */
         inline void clear() {entities.clear();}

         /*
            Returns the number of entities in the map.

            Input: (none)
            Output: size_t
         */
         inline const size_t size() const {return entities.size();}

         /*
            Returns a const iterator pointing to the beginning of the map. Used
            to iterate over all contained entities.

            Input: (none)
            Output: entityUnorderedMap::const_iterator const
         */
         inline entityUnorderedMap::const_iterator const begin() const {return entities.cbegin();}

         /*
            Returns a const iterator pointing to the end of the map. Used to
            iterate over all contained entities.

            Input: (none)
            Output: entityUnorderedMap::const_iterator const
         */
         inline entityUnorderedMap::const_iterator const end() const {return entities.cend();}
   };

   /***************************************************************************/

   class PlaceMap: public EntityMap {

      public:

         inline Place *get(const std::string name) {

            try {
               return dynamic_cast<Place *>(EntityMap::getSharedPtr(name).get());
            } catch (const bool &e) {
               return nullptr;
            }
         }
   };

   /***************************************************************************/

   class RoomMap: public PlaceMap {

      public:

         inline Room *get(const std::string name) {

            try {
               return dynamic_cast<Room *>(EntityMap::getSharedPtr(name).get());
            } catch (const bool &e) {
               return nullptr;
            }
         }
   };

   /***************************************************************************/

   class ThingMap: public EntityMap {

      public:

         inline Thing *get(const std::string name) {

            try {
               return dynamic_cast<Thing *>(EntityMap::getSharedPtr(name).get());
            } catch (const bool &e) {
               return nullptr;
            }
         }
   };

   /***************************************************************************/

   class BeingMap: public ThingMap {

      public:

         inline Being *get(const std::string name) {

            try {
               return dynamic_cast<Being *>(EntityMap::getSharedPtr(name).get());
            } catch (const bool &e) {
               return nullptr;
            }
         }
   };

   /***************************************************************************/

   class PlayerMap: public BeingMap {

      public:

         inline Player *get(const std::string name) {

            try {
               return dynamic_cast<Player *>(EntityMap::getSharedPtr(name).get());
            } catch (const bool &e) {
               return nullptr;
            }
         }
   };

   /***************************************************************************/

   class CreatureMap: public BeingMap {

      public:

         inline Creature *get(const std::string name) {

            try {
               return dynamic_cast<Creature *>(EntityMap::getSharedPtr(name).get());
            } catch (const bool &e) {
               return nullptr;
            }
         }
   };

   /***************************************************************************/

   class ObjectMap: public ThingMap {

      public:

         inline Object *get(const std::string name) {

            try {
               return dynamic_cast<Object *>(EntityMap::getSharedPtr(name).get());
            } catch (const bool &e) {
               return nullptr;
            }
         }
   };
}}


#endif
