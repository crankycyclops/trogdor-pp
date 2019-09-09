#ifndef ENTITYMAP_H
#define ENTITYMAP_H


#include <string>
#include <unordered_map>

#include "entities/entity.h"
#include "entities/place.h"
#include "entities/thing.h"
#include "entities/room.h"
#include "entities/being.h"
#include "entities/player.h"
#include "entities/creature.h"
#include "entities/object.h"


using namespace std;

namespace trogdor { namespace core { namespace entity {


   /*
      EntityMap wraps around the unordered_map type and maps names to entities.
      This class isn't strictly necessary, but I have a gut feeling that's
      telling me the extra abstraction will help me later when I do more with my
      engine.  We'll see...

      There are derived classes for each entity type, which go by the name of
      "<Type>Map."  These do nothing except wrap around the parent getter and
      cast the result to the more specific type.

      The entity map is utilized by the Parser when constructing game objects
      from the game.xml file, and also in the Game object to maintain a list of
      all the things that are in the game.
   */
   class EntityMap {

      private:

         // Hash table mapping names to entities
         unordered_map<string, Entity *> entities;

      public:

         /*
            Destructor for EntityMap.
         */
         inline ~EntityMap() {clear();}

         /* Returns true if the entity with the given name is set and false if
            it's not.

            Input: Entity name (string)
            Output: true if it's set and false if it's not
         */
         inline bool isset(string name) const {

            return entities.find(name) == entities.end() ? false : true;
         }

         /*
            Gets an entity by name.  If it doesn't exist, NULL will be
            returned.

            Input: Entity name (string)
            Output: Pointer to entity (Entity *)
         */
         inline Entity *get(const string name) {

            return isset(name) ? entities.find(name)->second : 0;
         }

         /*
            Sets an entity.  If the entity already exists, it will be
            overwritten.

            Input: Entity name (string), Pointer to entity
            Output: (none)
         */
         inline void set(string name, Entity *entity) {entities[name] = entity;}

         /*
            Removes an entity from the hash table.

            Input: Name of Entity
            Output: (none)
         */
         inline void erase(string name) {entities.erase(name);}

         /*
            Removes all entities from the EntityMap.  The entities themselves
            are not destroyed!

            Input: (none)
            Output: (none)
         */
         inline void clear() {entities.clear();}

         /*
            Destroys all contained Entities, then clears the hash table. We can't
            make this a part of the destructor, because there are cases where we
            want to free the memory associated with an EntityMap without freeing
            the Entities inside.

            Input: (none)
            Output: (none)
         */
         inline void destroyEntities() {

            for (unordered_map<string, Entity *>::iterator i = entities.begin();
            i != entities.end(); i++) {
               delete i->second;
            }

            clear();
         }
   };

   /***************************************************************************/

   class PlaceMap: public EntityMap {

      public:

         inline Place *get(const string name) {

            return dynamic_cast<Place *>(EntityMap::get(name));
         }
   };

   /***************************************************************************/

   class RoomMap: public PlaceMap {

      public:

         inline Room *get(const string name) {

            return dynamic_cast<Room *>(EntityMap::get(name));
         }
   };

   /***************************************************************************/

   class ThingMap: public EntityMap {

      public:

         inline Thing *get(const string name) {

            return dynamic_cast<Thing *>(EntityMap::get(name));
         }
   };

   /***************************************************************************/

   class BeingMap: public ThingMap {

      public:

         inline Being *get(const string name) {

            return dynamic_cast<Being *>(EntityMap::get(name));
         }
   };

   /***************************************************************************/

   class PlayerMap: public BeingMap {

      public:

         inline Player *get(const string name) {

            return dynamic_cast<Player *>(EntityMap::get(name));
         }
   };

   /***************************************************************************/

   class CreatureMap: public BeingMap {

      public:

         inline Creature *get(const string name) {

            return dynamic_cast<Creature *>(EntityMap::get(name));
         }
   };

   /***************************************************************************/

   class ObjectMap: public ThingMap {

      public:

         inline Object *get(const string name) {

            return dynamic_cast<Object *>(EntityMap::get(name));
         }
   };
}}}


#endif
