#ifndef INSTANTIATOR_H
#define INSTANTIATOR_H


#include <string>
#include <memory>
#include <algorithm>
#include <unordered_map>
#include <functional>

#include "../entities/room.h"
#include "../entities/object.h"
#include "../entities/creature.h"
#include "../entities/player.h"

#include "../vocabulary.h"
#include "../parser/ast.h"
#include "../exception/undefinedexception.h"


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
         typedef void (*validatorFunc) (const Vocabulary &vocabulary, std::string value);

         // Maps operation types to functions that should be executed before the
         // actual operation is performed
         std::unordered_map<ASTOperation,
            std::function<void(const std::shared_ptr<ASTOperationNode> &operation)>,
            std::hash<int>> preOperations;

         // Maps operation types to functions that perform the operation
         std::unordered_map<ASTOperation,
            std::function<void(const std::shared_ptr<ASTOperationNode> &operation)>,
            std::hash<int>> operations;

         // Maintains a list of valid Entity and Entity class properties (second
         // list's keys), along with a validator function for each (second
         // list's values) for each Entity type (first list's keys)
         std::unordered_map<std::string, std::unordered_map<std::string, validatorFunc>> entityPropValidators;

         // Maintains a list of valid Game properties (keys), along with a
         // validator function for each (values)
         std::unordered_map<std::string, validatorFunc> gamePropValidators;

         /*
            Throws an exception if the value is not a valid boolean.

            Input:
               Reference to vocabulary (unused)
               Property value (std::string)

            Output:
               (none)
         */
         static void assertBool(const Vocabulary &vocabulary, std::string value);

         /*
            Throws an exception if the value is not a valid integer.

            Input:
               Reference to vocabulary (unused)
               Property value (std::string)

            Output:
               (none)
         */
         static void assertInt(const Vocabulary &vocabulary, std::string value);

         /*
            Throws an exception if value is not a valid double precision number.

            Input:
               Reference to vocabulary (unused)
               Property value (std::string)

            Output:
               (none)
         */
         static void assertDouble(const Vocabulary &vocabulary, std::string value);

         /*
            Throws an exception if value does not represent a valid probability
            (number between 0 and 1.)

            Input:
               Reference to vocabulary (unused)
               Property value (std::string)

            Output:
               (none)
         */
         static void assertProbability(const Vocabulary &vocabulary, std::string value);

         /*
            A dummy validator that whitelists all strings.

            Input:
               Reference to vocabulary (unused)
               Property value (std::string)

            Output:
               Always returns true
         */
         static void assertString(const Vocabulary &vocabulary, std::string value);

         /*
            Asserts that the correct number of arguments were passed to an AST
            operation node.

            Input:
               Operation node (const std::shared_ptr<ASTOperationNode> &)
               Correct number of arguments (int)

            Output:
               (none)
         */
         void assertValidASTArguments(const std::shared_ptr<ASTOperationNode> &operation,
         int numArgs);

         /*
            Asserts that the correct number of arguments were passed to an AST
            operation node. This version of the function takes into account
            a variable number of arguments depending on the target type.

            Input:
               Operation node (const std::shared_ptr<ASTOperationNode> &)
               Minimum number of arguments (int)
               Mapping of target type to correct number of arguments

            Output:
               (none)
         */
         void assertValidASTArguments(const std::shared_ptr<ASTOperationNode> &operation,
         int minArgs, std::unordered_map<std::string, int> targetTypeToNumArgs);

         /*
            Asserts that the entity or entity class exists in the symbol table
            and throws an exception if not.

            Input:
               Target type: one of "entity" or "class" (std::string)
               Target name (std::string)
               Brief description of the action that was attempted for the error
                  message (std::string)
               Line number in source where operation originated (optional int)

            Output:
               (none)
         */
         void assertTargetExists(std::string targetType, std::string targetName,
         std::string action, int lineNumber = 0);

         /*
            Maps AST Operation type -> pre-operation function.

            Input:
               (none)

            Output:
               (none)
         */
         void mapPreOperations();

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

         // Reference to the game's vocabulary (read-only)
         const Vocabulary &vocabulary;

         // Record of a single declared Entity
         struct EntitySymbol {
            std::string name;
            std::string className;
            entity::EntityType type;
         };

         struct {

            // Defined Entities
            std::unordered_map<std::string, EntitySymbol> entities;

            // Defined Entity classes
            std::unordered_map<std::string, entity::EntityType> entityClasses;

         } symbols;

         struct {

            // Custom directions
            std::unordered_set<std::string> directions;

            // Maps synonyms to directions
            std::unordered_map<std::string, std::string> directionSynonyms;

            // Maps synonyms to verbs
            std::unordered_map<std::string, std::string> verbSynonyms;
         } customVocabulary;

         /*
            Registers an ASTOperation that the Instantiator can perform.

            Input:
               Operation type (ASTOperation)
               Operation function (std::function)

            Output:
               (none)
         */
         void registerOperation(ASTOperation operation,
         std::function<void(const std::shared_ptr<ASTOperationNode> &operation)> opFunc);

         /*
            Executes an operation represented by an ASTNode.

            Input:
               AST Node (const ASTNode &)

            Output:
               (none)
         */
         void executeOperation(const std::shared_ptr<ASTOperationNode> &operation);

         /*
            Optional hook that gets called at the end of instantiate(). If this
            isn't needed, you don't have to implement it in the derived class.

            Input:
               (none)

            Output:
               (none)
         */
         virtual void afterInstantiate();

      public:

         /*
            Parses the AST and instantiates the game. After instantiation, calls
            the optional hook afterInstantiate(), which does nothing unless
            implemented in a particular implementation of Instantiator.

            Input:
               AST root node (ASTNode)

            Output:
               (none)
         */
         void instantiate(const std::shared_ptr<ASTNode> &ast);

         // Constructor and assignment operator
         Instantiator& operator=(const Instantiator &) = delete;
         Instantiator(const Instantiator &) = delete;
         Instantiator() = delete;
         Instantiator(const Vocabulary &v);
   };
}


#endif
