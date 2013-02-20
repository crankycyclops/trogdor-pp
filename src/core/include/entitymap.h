#ifndef ENTITYMAP_H
#define ENTITYMAP_H


#include <string>
#include <boost/unordered_map.hpp>

#include "entities/entity.h"
#include "entities/place.h"
#include "entities/thing.h"
#include "entities/room.h"
#include "entities/being.h"
#include "entities/item.h"
#include "entities/player.h"
#include "entities/creature.h"
#include "entities/object.h"

using namespace std;
using namespace boost;


namespace core { namespace entity {


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
            Removes all entities from the EntityMap.  The entities themselves
            are not destroyed!

            Input: (none)
            Output: (none)
         */
         inline void clear() {entities.clear();}
   };

   /***************************************************************************/

   class PlaceMap: public EntityMap {

      inline Place *get(const string name) {

         return (Place *)EntityMap::get(name);
      }
   };

   /***************************************************************************/

   class RoomMap: public PlaceMap {

      inline Room *get(const string name) {

         return (Room *)EntityMap::get(name);
      }
   };

   /***************************************************************************/

   class ThingMap: public EntityMap {

      inline Thing *get(const string name) {

         return (Thing *)EntityMap::get(name);
      }
   };

   /***************************************************************************/

   class BeingMap: public ThingMap {

      inline Being *get(const string name) {

         return (Being *)EntityMap::get(name);
      }
   };

   /***************************************************************************/

   class PlayerMap: public BeingMap {

      inline Player *get(const string name) {

         return (Player *)EntityMap::get(name);
      }
   };

   /***************************************************************************/

   class CreatureMap: public BeingMap {

      inline Creature *get(const string name) {

         return (Creature *)EntityMap::get(name);
      }
   };

   /***************************************************************************/

   class ItemMap: public ThingMap {

      inline Item *get(const string name) {

         return (Item *)EntityMap::get(name);
      }
   };

   /***************************************************************************/

   class ObjectMap: public ItemMap {

      inline Object *get(const string name) {

         return (Object *)EntityMap::get(name);
      }
   };
}}


#endif
