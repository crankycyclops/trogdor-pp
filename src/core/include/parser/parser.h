#ifndef PARSER_H
#define PARSER_H


#include <memory>
#include <string>

#include "../vocabulary.h"
#include "../instantiator/instantiator.h"
#include "../exception/undefinedexception.h"

#include "data.h"


using namespace std;

namespace trogdor {


   /*
      Defines a standard interface for parsing game definitions. Provides an
      instance of Instantiator, which is responsible for creating the
      corresponding data structures.
   */
   class Parser {

      protected:

         // Reference to the game's vocabulary (for lookups)
         const Vocabulary &vocabulary;

         // When passed to a function, determines if we're parsing for an entity
         // class, an entity object, the default player, or the game. This makes
         // sure that we can call the proper instantiator method using wrapper
         // methods like entitySetter().
         enum ParseMode {
            PARSE_CLASS = 0,
            PARSE_ENTITY = 1,
            PARSE_DEFAULT_PLAYER = 2,
            PARSE_GAME = 3
         };

         // Used to instantiate entities and events
         std::unique_ptr<Instantiator> instantiator;

         /*
            Sets an Entity or Entity class's property via the instantiator.

            Input:
               Entity name (string, ignored if mode = PARSE_DEFAULT_PLAYER)
               Property name (string)
               Property value (string)
               Whether we're parsing aliases for Entity of Entity class (enum ParseMode mode)
                  . Possible values are: PARSE_ENTITY, PARSE_ENTITY_CLASS, and
                    PARSE_DEFAULT_PLAYER
         */
         inline void entitySetter(string entity, string property,
         string value, enum ParseMode mode) {

            switch (mode) {

               case PARSE_ENTITY:
                  instantiator->entitySetter(entity, property, value);
                  break;

               case PARSE_CLASS:
                  instantiator->entityClassSetter(entity, property, value);
                  break;

               case PARSE_DEFAULT_PLAYER:
                  instantiator->defaultPlayerSetter(property, value);
                  break;

               default:
                  throw UndefinedException("Parser::entitySetter: invalid mode. This is a bug.");
            }
         }

         /*
            Sets a message for the specified Entity or Entity class via the
            instantiator.

            Input:
               Entity name (string, ignored if mode = PARSE_DEFAULT_PLAYER)
               Property name (string)
               Property value (string)
               Whether we're parsing aliases for Entity of Entity class (enum ParseMode)
                  . Possible values for mode are PARSE_ENTITY, PARSE_CLASS, or
                    PARSE_DEFAULT_PLAYER.
         */
         inline void setEntityMessage(string entity, string messageName,
         string message, enum ParseMode mode) {

            switch (mode) {

               case PARSE_ENTITY:
                  instantiator->setEntityMessage(entity, messageName, message);
                  break;

               case PARSE_CLASS:
                  instantiator->setEntityClassMessage(entity, messageName, message);
                  break;

               case PARSE_DEFAULT_PLAYER:
                  instantiator->setDefaultPlayerMessage(messageName, message);
                  break;

               default:
                  throw UndefinedException("Parser::setEntityMessage: invalid parse mode. This is a bug.");
            }
         }

         /*
            Loads a Lua script into a Lua state belonging either to an entity,
            an entity class, or an instance of Game.

            Input:
               What kind of object we're loading the script into (enum ParseMode)
                  . Possible values are PARSE_ENTITY, PARSE_CLASS, or PARSE_GAME
               Script filename or content (string)
               Whether we're loading the script from a file or a string
                  (enum Instantiator::LoadScriptMethod)
               Entity or Entity class name (ignored if mode = PARSE_GAME)
         */
         inline void loadScript(enum ParseMode mode, string script,
         enum Instantiator::LoadScriptMethod scriptMethod = Instantiator::FILE,
         string entityName = "") {

            switch (mode) {

               case PARSE_ENTITY:
                  instantiator->loadEntityScript(entityName, script, scriptMethod);
                  break;

               case PARSE_CLASS:
                  instantiator->loadEntityClassScript(entityName, script, scriptMethod);
                  break;

               case PARSE_GAME:
                  instantiator->loadGameScript(script, scriptMethod);
                  break;

               default:
                  throw UndefinedException("Parser::loadScript: invalid parse mode. This is a bug.");
            }
         }

         /*
            Sets an event for Game, an Entity, or an Entity class using the
            Instantiator.

            Input:
               What kind of object we're loading the script into (enum ParseMode)
                  . Possible values are PARSE_ENTITY, PARSE_CLASS, or PARSE_GAME
               Event name (string)
               Lua function to call for the event (string)
               Entity or Entity class name (ignored if mode = PARSE_GAME)
         */
         inline void setEvent(enum ParseMode mode, string eventName, string function,
         string entityName = "") {

            switch (mode) {

               case PARSE_ENTITY:
                  instantiator->setEntityEvent(entityName, eventName, function);
                  break;

               case PARSE_CLASS:
                  instantiator->setEntityClassEvent(entityName, eventName, function);
                  break;

               case PARSE_GAME:
                  instantiator->setGameEvent(eventName, function);
                  break;

               default:
                  throw UndefinedException("Parser::setEvent: invalid parse mode. This is a bug.");
            }
         }

      public:

         /*
            Constructor for the Parser class.

            Input:
               Instantiator instance (std::unique_ptr<Intantiator>)
         */
         inline Parser(std::unique_ptr<Instantiator> i, const Vocabulary &v):
         vocabulary(v) {

            instantiator = std::move(i);
         }

         /*
            Parses a game definition and passes the data off to an instance of
            Instantiator, which creates the corresponding data structures.

            Input:
               Filename where the game definition is saved (string)

            Output:
               (none)
         */
         virtual void parse(string filename) = 0;
   };
}


#endif

