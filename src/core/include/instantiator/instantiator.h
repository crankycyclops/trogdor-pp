#ifndef INSTANTIATOR_H
#define INSTANTIATOR_H


#include "../entities/room.h"
#include "../entities/object.h"
#include "../entities/creature.h"
#include "../entities/player.h"


namespace trogdor { namespace core {

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

      public:

         // When passed to loadGameScript or loadEntityScript, determines
         // whether we're loading a script from a file or a string.
         enum LoadScriptMethod {
            FILE = 0,
            STRING = 1
         };

         /*
            Creates an entity class that can be used to instantiate one or
            more entities of a specific type.

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
            Set's an Entity class's property to the specified value. See
            comment for entitySetter() for more details.

            Input:
               Entity class's name (string)
               Property name (string)
               Property value (string)

            Output:
               (none)
         */
         virtual void entityClassSetter(string className, string property,
         string value) = 0;

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
         virtual void entitySetter(string entityName, string property,
         string value) = 0;

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
         virtual void defaultPlayerSetter(string property, string value) = 0;

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
         virtual void gameSetter(string property, string value) = 0;

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
}}


#endif

