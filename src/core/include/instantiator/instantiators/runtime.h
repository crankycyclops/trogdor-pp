#ifndef RUNTIME_H
#define RUNTIME_H


#include "../instantiator.h"
#include "../../game.h"


namespace trogdor { namespace core {

   class Runtime: public Instantiator {

      private:

         // Pointer to the game we're populating
         Game *game;

      public:

         /*
            Constructor
         */
         Runtime() = delete;
         Runtime(const Runtime &) = delete;
         inline Runtime(Game *g) {game = g;}

         /*
            Creates an entity class that can be used to instantiate one or
            more entities of a specific type.

            Input:
               Entity class's name (string)
               Entity class type (Room, Creature, or Object.)

            Output:
               (none)
         */
         virtual void makeEntityClass(string name, enum entity::EntityType);

         /*
            Set's an Entity class's property to the specified value. See
            comment for setEntityProperty() for more details.

            Input:
               Entity class's name (string)
               Property name (string)
               Property value (string)

            Output:
               (none)
         */
         virtual void setEntityClassProperty(string className, string property,
         string value);

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
         string className = "");

         /*
            Set's an Entity's property to the specified value. Properties are
            defined using periods to demarcate sections. So, for example, a
            Being's weight setting for their inventory would be "inventory.weight."
            It is the derived class's responsibility to infer the appropriate
            data type for each property.

            Input:
               Entity's name (string)
               Property name (string)
               Property value (string)

            Output:
               (none)
         */
         virtual void setEntityProperty(string entityName, string property,
         string value);

         /*
            Identical to setEntityProperty, except that we're setting a property
            of the default player.

            Input:
               Property name (string)
               Property value (string)

            Output:
               (none)
         */
         virtual void setDefaultPlayerProperty(string property, string value);

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
         virtual void loadGameScript(string script, enum LoadScriptMethod method = FILE);

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
         enum LoadScriptMethod method = FILE);

         /*
            Sets a global event.

            Input:
               Event name (string)
               Function to call when event is triggered(string)

            Output:
               (none)
         */
         virtual void setGameEvent(string eventName, string function);

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
         string function);

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
         string message);

         /*
            Not actually used by Runtime, but required by the parent class. Just
            fill this in as an empty method.

            Input:
               (none)

            Output:
               (none)
         */
         virtual void instantiate();
   };
}}


#endif

