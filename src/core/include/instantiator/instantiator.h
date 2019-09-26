#ifndef INSTANTIATOR_H
#define INSTANTIATOR_H


#include <string>
#include <unordered_map>

#include "../entities/room.h"
#include "../entities/object.h"
#include "../entities/creature.h"
#include "../entities/player.h"

#include "../exception/undefinedexception.h"


using namespace std;

namespace trogdor {


   /*
      The purpose of this class is to separate the parsing of entities and game
      settings from their actual instantiation. This allows me to decouple the
      parser from core::Game and to use it for other tasks. For example, one
      feature I hope to write is a "compiler" that parses game.xml into a CPP
      file that can be compiled into the game, allowing people to distribute
      standalone executables that don't rely on an easily read and easily
      modifiable configuration file.
   */
   class Instantiator {

      private:

         // Function type used to check if a property value is valid
         typedef void (*validatorFunc) (string value);

         // Maintains a list of valid Entity and Entity class properties (second
         // list's keys), along with a validator function for each (second
         // list's values) for each Entity type (first list's keys)
         unordered_map<string, unordered_map<string, validatorFunc>> entityPropValidators;

         // Maintains a list of valid Game properties (keys), along with a
         // validator function for each (values)
         unordered_map<string, validatorFunc> gamePropValidators;

         /*
            Throws an exception if the value is not a valid boolean.

            Input:
               Property value (string)

            Output:
               (none)
         */
         static void assertBool(string value);

         /*
            Throws an exception if the value is not a valid integer.

            Input:
               Property value (string)

            Output:
               (none)
         */
         static void assertInt(string value);

         /*
            Throws an exception if value is not a valid double precision number.

            Input:
               Property value (string)

            Output:
               (none)
         */
         static void assertDouble(string value);

         /*
            Throws an exception if value does not represent a valid probability
            (number between 0 and 1.)

            Input:
               Property value (string)

            Output:
               (none)
         */
         static void assertProbability(string value);

         /*
            A dummy validator that whitelists all strings.

            Input:
               Property value (string)

            Output:
               Always returns true
         */
         static void assertString(string value);

         /*
            Maps Entity type -> Entity property name -> Validator function.

            Input:
               (none)

            Output:
               (none)
         */
         void mapEntityPropValidators();

         /*
            Maps Game property name -> Validator function.

            Input:
               (none)

            Output:
               (none)
         */
         void mapGamePropValidators();

      protected:

         /*
            Does the actual setting of the Entity class's property value and
            throws an exception if there are any errors.

            Input:
               Entity class's name (string)
               Property name (string)
               Property value (string)

            Output:
               (none)
         */
         virtual void entityClassSetterDriver(string className, string property,
         string value) = 0;

         /*
            Does the actual setting of the Entity's property value and throws an
            exception if there are any errors.

            Input:
               Entity's name (string)
               Property name (string)
               Property value (string)

            Output:
               (none)
         */
         virtual void entitySetterDriver(string entityName, string property,
         string value) = 0;

         /*
            Does the actual setting of the default player property value and
            throws an exception if there are any errors.

            Input:
               Property name (string)
               Property value (string)

            Output:
               (none)
         */
         virtual void defaultPlayerSetterDriver(string property, string value) = 0;

         /*
            Does the actual setting of the game property value and throws an
            exception if there are any errors.

            Input:
               Property name (string)
               Property value (string)

            Output:
               (none)
         */
         virtual void gameSetterDriver(string property, string value) = 0;

      public:

         // When passed to loadGameScript or loadEntityScript, determines
         // whether we're loading a script from a file or a string.
         enum LoadScriptMethod {
            FILE = 0,
            STRING = 1
         };

         // Constructor and assignment operator
         Instantiator& operator=(const Instantiator &) = delete;
         Instantiator(const Instantiator &) = delete;
         Instantiator();

         /*
            Creates an entity class that can be used to instantiate one or
            more entities of a specific type.

            IMPORTANT: It is the implementor's responsibility to ensure the
            uniqueness of the class name per type.

            Input:
               Entity class's name (string)
               Entity class type (Room, Creature, or Object.)

            Output:
               (none)
         */
         virtual void makeEntityClass(string name, enum entity::EntityType) = 0;

         /*
            Returns true if the entity class identified by className exists and
            false if either it doesn't or, if it does, the entity type doesn't
            match.

            Input:
               Entity class's name (string)
               Entity class's type (enum entity::entityType)

            Output:
               Whether or not the Entity class exists (bool)
         */
         virtual bool entityClassExists(string className,
         enum entity::EntityType entityType) = 0;

         /*
            Same as previous definition of entityClassExists() except that it
            doesn't take into account the class's type.

            Input:
               Entity's class name (string)

            Output:
               Whether or not the Entity class exists (bool)
         */
         virtual bool entityClassExists(string className) = 0;

         /*
            Takes as input the name of an Entity class and returns its type.

            Input:
               Entity class's name (string)

            Output:
               Entity class's type (enum Entity::EntityType)
         */
         virtual enum entity::EntityType getEntityClassType(string className) = 0;

         /*
            Set's an Entity class's property to the specified value. See
            comment for entitySetter() for more details.

            Input:
               Entity class's name (string)
               Property name (string)
               Property value (string)

            Output:
               (none)
         */
         inline void entityClassSetter(string className, string property,
         string value) {

            string classType;

            if (!entityClassExists(className)) {
               throw UndefinedException(
                  string("Instantiator::entityClassSetter: Entity class '")
                  + className + "' does not exist. This is a bug."
               );
            }

            classType = entity::Entity::typeToStr(getEntityClassType(className));

            if (entityPropValidators.find(classType) == entityPropValidators.end()) {
               throw UndefinedException(
                  string("Instantiator::entityClassSetter: Setting property ")
                  + "on Entity with unsupported type. This is a bug."
               );
            }

            else if (entityPropValidators[classType].end() ==
            entityPropValidators[classType].find(property)) {
               throw UndefinedException(
                  string("Instantiator::entityClassSetter: Setting unsupported ")
                  + classType + " property '" + property + "'. This is a bug."
               );
            }

            entityPropValidators[classType][property](value);
            entityClassSetterDriver(className, property, value);
         }

         /*
            Identical to loadGameScript and loadEntityScript, except that the
            script gets loaded into an Entity class's Lua state.

            Input:
               Entity class's name (string)
               Script (filename or script)
               Load method (FILE = load by filename, STRING = load as string)

            Output:
               (none)
         */
         virtual void loadEntityClassScript(string entityClass, string script,
         enum LoadScriptMethod method = FILE) = 0;

         /*
            Sets an event for a specific Entity class.

            Input:
               Entity class name (string)
               Event name (string)
               Function to call when event is triggered(string)

            Output:
               (none)
         */
         virtual void setEntityClassEvent(string entityName, string eventName,
         string function) = 0;

         /*
            Set's a message for an Entity class.

            Input:
               Entity class's name (string)
               Message name (string)
               Message value (string)

            Output:
               (none)
         */
         virtual void setEntityClassMessage(string className, string messageName,
         string message) = 0;

         /*
            Instantiates an entity (Room, Creature, or Object.) If a class is
            specified, the entity will be configured according to that class's
            properties.

            IMPORTANT: It is the implementor's responsibility to ensure that no
            other Entity exists with the same name.

            Input:
               Entity's name (string)
               Entity type (Room, Creature, or Object.)
               Entity's class (string, optional)

            Output:
               (none)
         */
         virtual void makeEntity(string entityName, enum entity::EntityType type,
         string className = "") = 0;

         /*
            Returns true if the entity identified by entityName exists and false
            if it doesn't.

            Input:
               Entity's name (string)

            Output:
               Whether or not the Entity exists (bool)
         */
         virtual bool entityExists(string entityName) = 0;

         /*
            Returns the type of the Entity referenced by entityName. Throws an
            exception if the Entity doesn't exist.

            Input:
               Entity's name (string)

            Output:
               The Entity's type (enum entity::EntityType)
         */
         virtual enum entity::EntityType getEntityType(string entityName) = 0;

         /*
            Returns the class of the Entity referenced by entityName. Throws an
            exception if the Entity doesn't exist.

            Input:
               Entity's name (string)

            Output:
               The Entity's class (string)
         */
         virtual string getEntityClass(string entityName) = 0;

         /*
            Calls an Entity's setter. Setter names are defined using periods to
            demarcate sections. So, for example, a Being's weight setting for
            their inventory would be "inventory.weight." It is the derived
            class's responsibility to infer the appropriate data type for each
            property.

            Input:
               Entity's name (string)
               Property name (string)
               Property value (string)

            Output:
               (none)
         */
         inline void entitySetter(string entityName, string property,
         string value) {

            string entityType;

            if (!entityExists(entityName)) {
               throw UndefinedException(
                  string("Instantiator::entitySetter: Entity '") + entityName
                  + "' does not exist. This is a bug."
               );
            }

            entityType = entity::Entity::typeToStr(getEntityType(entityName));

            if (entityPropValidators.find(entityType) ==
            entityPropValidators.end()) {
               throw UndefinedException(
                  string("Instantiator::entitySetter: Setting property on ")
                  + "unsupported Entity type. This is a bug."
               );
            }

            else if (entityPropValidators[entityType].end() ==
            entityPropValidators[entityType].find(property)) {
               throw UndefinedException(
                  string("Instantiator::entitySetter: Setting unsupported ")
                  + entityType + " property '" + property + "'. This is a bug."
               );
            }

            entityPropValidators[entityType][property](value);
            entitySetterDriver(entityName, property, value);
         }

         /*
            Identical to loadGameScript, except that the script gets loaded into
            an Entity's Lua state instead of core::Game's.

            Input:
               Entity's name (string)
               Script (filename or script)
               Load method (FILE = load by filename, STRING = load as string)

            Output:
               (none)
         */
         virtual void loadEntityScript(string entityName, string script,
         enum LoadScriptMethod method = FILE) = 0;

         /*
            Sets an event for a specific Entity.

            Input:
               Entity name (string)
               Event name (string)
               Function to call when event is triggered(string)

            Output:
               (none)
         */
         virtual void setEntityEvent(string entityName, string eventName,
         string function) = 0;

         /*
            Set's a message for an Entity.

            Input:
               Entity's name (string)
               Message name (string)
               Message value (string)

            Output:
               (none)
         */
         virtual void setEntityMessage(string entityName, string messageName,
         string message) = 0;

         /*
            Identical to entitySetter, except that we're setting a property
            of the default player.

            Input:
               Property name (string)
               Property value (string)

            Output:
               (none)
         */
         inline void defaultPlayerSetter(string property, string value) {

            if (entityPropValidators["player"].end() ==
            entityPropValidators["player"].find(property)) {
               throw UndefinedException(
                  string("Instantiator::defaultPlayerSetter: Setting unsupported ")
                  + "player property '" + property + "'. This is a bug."
               );
            }

            entityPropValidators["player"][property](value);
            defaultPlayerSetterDriver(property, value);
         }

         /*
            Set's a message for the default player.

            Input:
               Message name (string)
               Message value (string)

            Output:
               (none)
         */
         virtual void setDefaultPlayerMessage(string messageName, string message) = 0;

         /*
            Similar to entitySetter, except that we're setting a property
            of the Game object.

            Input:
               Property we're setting (string)
               Property's value (string)

            Output:
               (none)
         */
         inline void gameSetter(string property, string value) {

            if (gamePropValidators.end() == gamePropValidators.find(property)) {
               throw UndefinedException(
                  string("Instantiator::gameSetter: Setting unsupported ")
                  + "game property '" + property + "'. This is a bug."
               );
            }

            gamePropValidators[property](value);
            gameSetterDriver(property, value);
         }

         /*
            Loads a Lua script into the game's global Lua state. By default,
            we load a script by filename, but passing STRING into the second
            argument will allow you to pass the script into the first parameter
            as a string.

            Input:
               Script (filename or script)
               Load method (FILE = load by filename, STRING = load as string)

            Output:
               (none)
         */
         virtual void loadGameScript(string script, enum LoadScriptMethod method = FILE) = 0;

         /*
            Sets a global event.

            Input:
               Event name (string)
               Function to call when event is triggered(string)

            Output:
               (none)
         */
         virtual void setGameEvent(string eventName, string function) = 0;

         /*
            Should be called after parsing is complete. If this step is
            unnecessary (as is the case for the default Runtime instantiator,
            which sets up entities and game settings during the above function
            calls), this can be set to an empty function.

            Input:
               (none)

            Output:
               (none)
         */
         virtual void instantiate() = 0;
   };
}


#endif

