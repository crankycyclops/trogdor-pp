#ifndef ENTITYCLASS_H
#define ENTITYCLASS_H


#include <memory>
#include <unordered_map>

#include "../entities/room.h"
#include "../entities/object.h"
#include "../entities/creature.h"


using namespace std;

namespace trogdor { namespace core { namespace entity {

      class EntityClass {

         private:

            // Entity classes are implemented as Entities with a defined set of
            // attributes which can then be cloned
            unordered_map<string, std::unique_ptr<Room>> RoomTypes;
            unordered_map<string, std::unique_ptr<Object>> ObjectTypes;
            unordered_map<string, std::unique_ptr<Creature>> CreatureTypes;

         public:

            /*
              Returns true if the specified Room class exists and false if not.

              Input:
                 name of class (string)

              Output:
                 bool
            */
            inline bool roomTypeExists(string name) {

               return RoomTypes.find(name) == RoomTypes.end() ? false : true;
            }

            /*
              Returns true if the specified Object class exists and false if not.

              Input:
                 name of class (string)

              Output:
                 bool
            */
            inline bool objectTypeExists(string name) {

               return ObjectTypes.find(name) == ObjectTypes.end() ? false : true;
            }

            /*
              Returns true if the specified Creature class exists and false if not.

              Input:
                 name of class (string)

              Output:
                 bool
            */
            inline bool creatureTypeExists(string name) {

               return CreatureTypes.find(name) == CreatureTypes.end() ? false : true;
            }

            /*
              Returns the Room class definition of the specified name. If it
              doesn't exist, returns a NULL pointer instead.

              Input:
                 name of class (string)

              Output:
                 Room *

              Exceptions:
                 (none)
            */
            inline Room *getRoomType(string name) {

               try {
                  return RoomTypes.at(name).get();
               }

               catch (const out_of_range &e) {
                  return 0;
               }
            }

            /*
              Returns the Object class definition of the specified name. If it
              doesn't exist, returns a NULL pointer instead.

              Input:
                 name of class (string)

              Output:
                 Object *

              Exceptions:
                 (none)
            */
            inline Object *getObjectType(string name) {

               try {
                  return ObjectTypes.at(name).get();
               }

               catch (const out_of_range &e) {
                  return 0;
               }
            }

            /*
              Returns the Creature class definition of the specified name. If it
              doesn't exist, returns a NULL pointer instead.

              Input:
                 name of class (string)

              Output:
                 Creature *

              Exceptions:
                 (none)
            */
            inline Creature *getCreatureType(string name) {

               try {
                  return CreatureTypes.at(name).get();
               }

               catch (const out_of_range &e) {
                  return 0;
               }
            }

            /*
              Inserts a Room's class definition of the specified name. An
              exception is thrown if a class with that name already exists.

              Input:
                 name of class (string)
                 class definition (std::unique_ptr<Room>)

              Output:
                 (none)

              Exceptions:
                 boolean true (if class with same name already exists)
            */
            inline void insertType(string name, std::unique_ptr<Room> r) {

               // make sure class with the same name doesn't already exist
               try {
                  RoomTypes.at(name);
                  throw true;
               }

               catch (const out_of_range &e) {
                  RoomTypes[name] = std::move(r);
               }
            }

            /*
              Inserts an Object's class definition of the specified name. An
              exception is thrown if a class with that name already exists.

              Input:
                 name of class (string)
                 class definition (std::unique_ptr<Object>)

              Output:
                 (none)

              Exceptions:
                 boolean true (if class with same name already exists)
            */
            inline void insertType(string name, std::unique_ptr<Object> o) {

               // make sure class with the same name doesn't already exist
               try {
                  ObjectTypes.at(name);
                  throw true;
               }

               catch (const out_of_range &e) {
                  ObjectTypes[name] = std::move(o);
               }
            }

            /*
              Inserts a Creature's class definition of the specified name. An
              exception is thrown if a class with that name already exists.

              Input:
                 name of class (string)
                 class definition (std::unique_ptr<Creature>)

              Output:
                 (none)

              Exceptions:
                 boolean true (if class with same name already exists)
            */
            inline void insertType(string name, std::unique_ptr<Creature> c) {

               // make sure class with the same name doesn't already exist
               try {
                  CreatureTypes.at(name);
                  throw true;
               }

               catch (const out_of_range &e) {
                  CreatureTypes[name] = std::move(c);
               }
            }
	};
}}}

#endif

