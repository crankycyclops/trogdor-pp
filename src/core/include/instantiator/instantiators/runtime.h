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

         // Function type used to set game properties via a SET_PROPERTY operation
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
         unordered_map<string, std::unique_ptr<entity::Entity>> typeClasses;

         // A hash mapping of entity type -> property name -> setter function
         unordered_map<string, unordered_map<string, propSetterFunc>> propSetters;

         // A hash mapping of game property setter functions
         unordered_map<string, gameSetterFunc> gameSetters;

         /*
            Registers AST operations that the Runtime Instantiator knows how to
            execute.

            Input:
               (none)

            Output:
               (none)
         */
         void registerOperations();

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
            implement the SET_PROPERTY operation.

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
         Runtime(const Vocabulary &v, Game *g);

         /*
            There are a few things we need to do after instantiation to get the
            game ready for play.

            Input:
               (none)

            Output:
               (none)
         */
         virtual void afterInstantiate();
   };
}


#endif

