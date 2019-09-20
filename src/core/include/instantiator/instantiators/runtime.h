#ifndef RUNTIME_H
#define RUNTIME_H


#include <memory>
#include <unordered_map>

#include "../instantiator.h"


namespace trogdor {

   // Forward declaration of Game, later defined in game.h
   class Game;

   class Runtime: public Instantiator {

      private:

         // Function type used by gameSetters on a Game object
         typedef void (*gameSetterFunc) (
            Game *game, string value
         );

         // Function type used by propSetters on Entities
         typedef void (*propSetterFunc) (
            Game *game, entity::Entity * entity, string value
         );

         // Pointer to the game we're populating
         Game *game;

         // User defined Entity classes
         unordered_map<string, std::unique_ptr<Entity>> typeClasses;

         // A hash mapping of entity type -> property name -> setter function
         unordered_map<string, unordered_map<string, propSetterFunc>> propSetters;

         // A hash mapping of game property setter functions
         unordered_map<string, gameSetterFunc> gameSetters;

         /*
            Creates a property -> function map that's used to
            implement gameSetter().

            Input:
               (none)

            Output:
               (none)
         */
         void mapGameSetters();

         /*
            Creates an entity type -> property -> function map that's used to
            implement entitySetter() and entityClassSetter().

            Input:
               (none)

            Output:
               (none)
         */
         void mapEntitySetters();

      public:

         /*
            Constructor
         */
         Runtime() = delete;
         Runtime(const Runtime &) = delete;
         Runtime(Game *g);

         /*
            Creates an entity class that can be used to instantiate one or
            more entities of a specific type.

            Input:
               Entity class's name (string)
               Entity class type (Room, Creature, or Object.)

            Output:
               (none)
         */
         virtual void makeEntityClass(string className, enum entity::EntityType classType);

         /*
            Returns true if the entity class identified by className exists and
            false if it doesn't.

            Input:
               Entity class's name (string)
               Entity class's type (enum entity::EntityType)

            Output:
               Whether or not the Entity class exists (bool)
         */
         virtual bool entityClassExists(string className,
         enum entity::EntityType entityType);

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
         string value);

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
         enum LoadScriptMethod method = FILE);

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
         string function);

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
         string message);

         /*
            Instantiates an entity (Room, Creature, or Object.) If a class is
            specified, the entity will be configured according to that class's
            properties.

            This method should throw an exception if another entity by the same
            name already exists.

            Input:
               Entity's name (string)
               Entity type (Room, Creature, or Object.)
               Entity's class (string, optional)

            Output:
               (none)
         */
         virtual void makeEntity(string entityName, enum entity::EntityType entityType,
         string className = "");

         /*
            Returns true if the entity identified by entityName exists and false
            if it doesn't.

            Input:
               Entity's name (string)

            Output:
               Whether or not the Entity exists (bool)
         */
         virtual bool entityExists(string entityName);

         /*
            Returns the type of Entity referenced by entityName. Throws an
            exception if the entity doesn't exist.

            Input:
               Entity's name (string)

            Output:
               The Entity's type (enum entity::EntityType)
         */
         virtual enum entity::EntityType getEntityType(string entityName);

         /*
            Returns the class of the Entity referenced by entityName. Throws an
            exception if the Entity doesn't exist.

            Input:
               Entity's name (string)

            Output:
               The Entity's class (string)
         */
         virtual string getEntityClass(string entityName);

         /*
            Calls an Entity's setter. Setter names are defined using periods to
            demarcate sections. So, for example, a Being's weight setting for
            their inventory would be "inventory.weight."

            Input:
               Entity's name (string)
               Property name (string)
               Property value (string)

            Output:
               (none)
         */
         virtual void entitySetter(string entityName, string property,
         string value);

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
            Identical to entitySetter, except that we're setting a property
            of the default player.

            Input:
               Property name (string)
               Property value (string)

            Output:
               (none)
         */
         virtual void defaultPlayerSetter(string property, string value);

         /*
            Set's a message for the default player.

            Input:
               Message name (string)
               Message value (string)

            Output:
               (none)
         */
         virtual void setDefaultPlayerMessage(string messageName, string message);

         /*
            Similar to entitySetter, except that we're setting a property
            of the Game object.

            Input:
               Property we're setting (string)
               Property's value (string)

            Output:
               (none)
         */
         virtual void gameSetter(string property, string value);

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
            Sets a global event.

            Input:
               Event name (string)
               Function to call when event is triggered(string)

            Output:
               (none)
         */
         virtual void setGameEvent(string eventName, string function);

         /*
            Most of the instantiation is completed already by the above functions,
            but there are a few things we need to do at the end before the game
            can be ready to play.

            Input:
               (none)

            Output:
               (none)
         */
         virtual void instantiate();
   };
}


#endif

