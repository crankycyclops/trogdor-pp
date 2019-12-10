#ifndef PARSER_H
#define PARSER_H


#include <memory>
#include <string>

#include "../vocabulary.h"
#include "../instantiator/instantiator.h"
#include "../exception/undefinedexception.h"

#include "data.h"
#include "ast.h"


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

         // Used to instantiate entities and events
         std::unique_ptr<Instantiator> instantiator;

         // The game gets parsed into an abstract syntax, which is then used to
         // instantiate the game
         std::shared_ptr<ASTNode> ast;

         /*
            Returns an AST subtree representing a defineDirection operation.

            Input:
               New direction (string)
               Current line number in the source being parsed (int)

            Output:
               ASTOperationNode
         */
         std::shared_ptr<ASTOperationNode> ASTDefineDirection(string direction,
         int lineNumber = 0);

         /*
            Returns an AST subtree representing a defineDirectionSynonym
            operation.

            Input:
               Direction (string)
               Synonym (string)
               Current line number in the source being parsed (int)

            Output:
               ASTOperationNode
         */
         std::shared_ptr<ASTOperationNode> ASTDefineDirectionSynonym(string direction,
         string synonym, int lineNumber = 0);

         /*
            Returns an AST subtree representing a defineVerbSynonym operation.

            Input:
               Original verb (string)
               Synonym (string)
               Current line number in the source being parsed (int)

            Output:
               ASTOperationNode
         */
         std::shared_ptr<ASTOperationNode> ASTDefineVerbSynonym(string verb,
         string synonym, int lineNumber = 0);

         /*
            Returns an AST subtree representing a defineEntityClass operation.

            Input:
               Class name (string)
               Entity type (string)
               Current line number in the source being parsed (int)

            Output:
               ASTOperationNode
         */
         std::shared_ptr<ASTOperationNode> ASTDefineEntityClass(string className,
         string entityType, int lineNumber = 0);

         /*
            Returns an AST subtree representing a defineEntity operation.

            Input:
               Entity name (string)
               Entity type (string)
               Class name (string)
               Current line number in the source being parsed (int)

            Output:
               ASTOperationNode
         */
         std::shared_ptr<ASTOperationNode> ASTDefineEntity(string entityName,
         string entityType, string className, int lineNumber = 0);

         /*
            Returns an AST subtree representing a setMessage operation.

            Input:
               Target type: the kind of thing we're setting a message on (string)
                  . One of: "entity", "class", or "defaultPlayer"
               Message name (string)
               Message value (string)
               Current line number in the source being parsed (int)
               Entity or entity class name (string)
                  . Do not pass if target type is "defaultPlayer"

            Output:
               ASTOperationNode
         */
         std::shared_ptr<ASTOperationNode> ASTSetMessage(string targetType,
         string messageName, string message, int lineNumber = 0,
         string entityOrClassName = "");

         /*
            Returns an AST subtree representing a setTag operation.

            Input:
               Target type: the kind of thing we're setting a tag on (string)
                  . One of: "entity", "class", or "defaultPlayer"
               Tag (string)
               Current line number in the source being parsed (int)
               Entity or entity class name (string)
                  . Do not pass if target type is "defaultPlayer"

            Output:
               ASTOperationNode
         */
         std::shared_ptr<ASTOperationNode> ASTSetTag(string targetType,
         string tag, int lineNumber = 0, string entityOrClassName = "");

         /*
            Returns an AST subtree representing a removeTag operation.

            Input:
               Target type: the kind of thing we're removing the tag on (string)
                  . One of: "entity", "class", or "defaultPlayer"
               Tag (string)
               Current line number in the source being parsed (int)
               Entity or entity class name (string)
                  . Do not pass if target type is "defaultPlayer"

            Output:
               ASTOperationNode
         */
         std::shared_ptr<ASTOperationNode> ASTRemoveTag(string targetType,
         string tag, int lineNumber = 0, string entityOrClassName = "");

         /*
            Returns an AST subtree representing a loadScript operation.

            Input:
               Target type: the kind of thing we're removing the tag on (string)
                  . One of: "entity", "class", or "game"
               Script mode (string)
                  . One of: "file" (to load a file) or "string" (to read a raw script)
               Script (string)
                  . Either a filename or the script itself, depending on scriptMode
               Current line number in the source being parsed (int)
               Entity or entity class name (string)
                  . Do not pass if target type is "game"

            Output:
               ASTOperationNode
         */
         std::shared_ptr<ASTOperationNode> ASTLoadScript(string targetType,
         string scriptMode, string script, int lineNumber = 0,
         string entityOrClassName = "");

         /*
            Returns an AST subtree representing a setEvent operation.

            Input:
               Target type: the kind of thing we're removing the tag on (string)
                  . One of: "entity", "class", or "game"
               Event that triggers the Lua function (string)
               Lua function to trigger (string)
               Current line number in the source being parsed (int)
               Entity or entity class name (string)
                  . Do not pass if target type is "game"

            Output:
               ASTOperationNode
         */
         std::shared_ptr<ASTOperationNode> ASTSetEvent(string targetType,
         string eventName, string luaFunction, int lineNumber = 0,
         string entityOrClassName = "");

         /*
            Returns an AST subtree representing a setAlias operation.

            Input:
               Target type: the kind of thing we're setting a meta value on (string)
                  . One of: "entity" or "class"
               Alias (string)
               Entity or entity class name (string)
               Current line number in the source being parsed (int)

            Output:
               ASTOperationNode
         */
         std::shared_ptr<ASTOperationNode> ASTSetAlias(string targetType,
         string alias, string thingOrClassName, int lineNumber = 0);

         /*
            Returns an AST subtree representing a setMeta operation.

            Input:
               Target type: the kind of thing we're setting a meta value on (string)
                  . One of: "entity", "class", "defaultPlayer", or "game"
               Meta key (string)
               Meta value (string)
               Current line number in the source being parsed (int)
               Entity or entity class name (string)
                  . Do not pass if target type is "game" or "defaultPlayer"

            Output:
               ASTOperationNode
         */
         std::shared_ptr<ASTOperationNode> ASTSetMeta(string targetType,
         string metaKey, string metaValue, int lineNumber = 0,
         string entityOrClassName = "");

         /*
            Returns an AST subtree representing a setAttribute operation.

            Input:
               Target type: the kind of thing we're setting a property on (string)
                  . One of: "entity", "class", or "game"
               Attribute name (string)
               Attribute value (string)
               Current line number in the source being parsed (int)
               Entity or entity class name (string)
                  . Do not pass if target type is "game"

            Output:
               ASTOperationNode
         */
         std::shared_ptr<ASTOperationNode> ASTSetAttribute(string targetType,
         string attribute, string value, int lineNumber = 0,
         string entityOrClassName = "");

         /*
            Returns an AST subtree representing a setProperty operation.

            Input:
               Target type: the kind of thing we're setting a property on (string)
                  . One of: "entity", "class", or "game"
               Property name (string)
               Property value (string)
               Current line number in the source being parsed (int)
               Entity or entity class name (string)
                  . Do not pass if target type is "game"

            Output:
               ASTOperationNode
         */
         std::shared_ptr<ASTOperationNode> ASTSetProperty(string targetType,
         string property, string value, int lineNumber = 0,
         string entityOrClassName = "");

         /*
            Returns an AST subtree representing an insertIntoInventory operation.

            Please note that inserting an item into the default player's
            inventory is not allowed, since multiple players can be cloned from
            defaultPlayer and an Object can only be in one inventory at a time.
            The same reasoning also makes it illegal to place an Object into
            a Being class's inventory.

            Input:
               Object name (string)
               Being name (string) -- classes not allowed
               Current line number in the source being parsed (int)

            Output:
               ASTOperationNode
         */
         std::shared_ptr<ASTOperationNode> ASTInsertIntoInventory(string objectName,
         string beingName, int lineNumber = 0);

         /*
            Returns an AST subtree representing an insertIntoRoom operation.

            Input:
               Object, Player, or Creature name (string) -- classes not allowed
               Room name (string)
               Current line number in the source being parsed (int)

            Output:
               ASTOperationNode
         */
         std::shared_ptr<ASTOperationNode> ASTInsertIntoRoom(string thing,
         string room, int lineNumber = 0);

         /*
            Returns an AST subtree representing a connectRooms operation.

            Input:
               Target type: the kind of thing we're setting the connection on (string)
                  . One of: "entity" or "class"
               Source room (string)
               Room source connects to (string)
               Direction where connection should be made (string)
               Current line number in the source being parsed (int)

            Output:
               ASTOperationNode
         */
         std::shared_ptr<ASTOperationNode> ASTConnectRooms(string targetType,
         string sourceRoomOrClass, string connectTo, string direction,
         int lineNumber = 0);

      public:

         /*
            When a class has one or more virtual functions, it should also have
            a virtual destructor.
         */
         virtual ~Parser() = 0;

         /*
            Constructor for the Parser class.

            Input:
               Instantiator instance (std::unique_ptr<Intantiator>)
         */
         inline Parser(std::unique_ptr<Instantiator> i, const Vocabulary &v):
         vocabulary(v) {

            ast = std::make_shared<ASTNode>();
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

