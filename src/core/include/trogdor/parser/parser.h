#ifndef PARSER_H
#define PARSER_H


#include <memory>
#include <string>

#include <trogdor/vocabulary.h>
#include <trogdor/instantiator/instantiator.h>
#include <trogdor/exception/undefinedexception.h>

#include <trogdor/parser/data.h>
#include <trogdor/parser/ast.h>


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
               New direction (std::string)
               Current line number in the source being parsed (int)

            Output:
               ASTOperationNode
         */
         std::shared_ptr<ASTOperationNode> ASTDefineDirection(std::string direction,
         int lineNumber = 0);

         /*
            Returns an AST subtree representing a defineDirectionSynonym
            operation.

            Input:
               Direction (std::string)
               Synonym (std::string)
               Current line number in the source being parsed (int)

            Output:
               ASTOperationNode
         */
         std::shared_ptr<ASTOperationNode> ASTDefineDirectionSynonym(std::string direction,
         std::string synonym, int lineNumber = 0);

         /*
            Returns an AST subtree representing a defineVerbSynonym operation.

            Input:
               Original verb (std::string)
               Synonym (std::string)
               Current line number in the source being parsed (int)

            Output:
               ASTOperationNode
         */
         std::shared_ptr<ASTOperationNode> ASTDefineVerbSynonym(std::string verb,
         std::string synonym, int lineNumber = 0);

         /*
            Returns an AST subtree representing a defineEntityClass operation.

            Input:
               Class name (std::string)
               Entity type (std::string)
               Current line number in the source being parsed (int)

            Output:
               ASTOperationNode
         */
         std::shared_ptr<ASTOperationNode> ASTDefineEntityClass(std::string className,
         std::string entityType, int lineNumber = 0);

         /*
            Returns an AST subtree representing a defineEntity operation.

            Input:
               Entity name (std::string)
               Entity type (std::string)
               Class name (std::string)
               Current line number in the source being parsed (int)

            Output:
               ASTOperationNode
         */
         std::shared_ptr<ASTOperationNode> ASTDefineEntity(std::string entityName,
         std::string entityType, std::string className, int lineNumber = 0);

         /*
            Returns an AST subtree representing a setMessage operation.

            Input:
               Target type: the kind of thing we're setting a message on (std::string)
                  . One of: "entity", "class", or "defaultPlayer"
               Message name (std::string)
               Message value (std::string)
               Current line number in the source being parsed (int)
               Entity or entity class name (std::string)
                  . Do not pass if target type is "defaultPlayer"

            Output:
               ASTOperationNode
         */
         std::shared_ptr<ASTOperationNode> ASTSetMessage(std::string targetType,
         std::string messageName, std::string message, int lineNumber = 0,
         std::string entityOrClassName = "");

         /*
            Returns an AST subtree representing a setTag operation.

            Input:
               Target type: the kind of thing we're setting a tag on (std::string)
                  . One of: "entity", "class", or "defaultPlayer"
               Tag (std::string)
               Current line number in the source being parsed (int)
               Entity or entity class name (std::string)
                  . Do not pass if target type is "defaultPlayer"

            Output:
               ASTOperationNode
         */
         std::shared_ptr<ASTOperationNode> ASTSetTag(std::string targetType,
         std::string tag, int lineNumber = 0, std::string entityOrClassName = "");

         /*
            Returns an AST subtree representing a removeTag operation.

            Input:
               Target type: the kind of thing we're removing the tag on (std::string)
                  . One of: "entity", "class", or "defaultPlayer"
               Tag (std::string)
               Current line number in the source being parsed (int)
               Entity or entity class name (std::string)
                  . Do not pass if target type is "defaultPlayer"

            Output:
               ASTOperationNode
         */
         std::shared_ptr<ASTOperationNode> ASTRemoveTag(std::string targetType,
         std::string tag, int lineNumber = 0, std::string entityOrClassName = "");

         /*
            Returns an AST subtree representing a loadScript operation.

            Input:
               Target type: the kind of thing we're removing the tag on (std::string)
                  . One of: "entity", "class", or "game"
               Script mode (std::string)
                  . One of: "file" (to load a file) or "string" (to read a raw script)
               Script (std::string)
                  . Either a filename or the script itself, depending on scriptMode
               Current line number in the source being parsed (int)
               Entity or entity class name (std::string)
                  . Do not pass if target type is "game"

            Output:
               ASTOperationNode
         */
         std::shared_ptr<ASTOperationNode> ASTLoadScript(std::string targetType,
         std::string scriptMode, std::string script, int lineNumber = 0,
         std::string entityOrClassName = "");

         /*
            Returns an AST subtree representing a setEvent operation.

            Input:
               Target type: the kind of thing we're removing the tag on (std::string)
                  . One of: "entity", "class", or "game"
               Event that triggers the Lua function (std::string)
               Lua function to trigger (std::string)
               Current line number in the source being parsed (int)
               Entity or entity class name (std::string)
                  . Do not pass if target type is "game"

            Output:
               ASTOperationNode
         */
         std::shared_ptr<ASTOperationNode> ASTSetEvent(std::string targetType,
         std::string eventName, std::string luaFunction, int lineNumber = 0,
         std::string entityOrClassName = "");

         /*
            Returns an AST subtree representing a setAlias operation.

            Input:
               Target type: the kind of thing we're setting a meta value on (std::string)
                  . One of: "entity" or "class"
               Alias (std::string)
               Entity or entity class name (std::string)
               Current line number in the source being parsed (int)

            Output:
               ASTOperationNode
         */
         std::shared_ptr<ASTOperationNode> ASTSetAlias(std::string targetType,
         std::string alias, std::string thingOrClassName, int lineNumber = 0);

         /*
            Returns an AST subtree representing a setMeta operation.

            Input:
               Target type: the kind of thing we're setting a meta value on (std::string)
                  . One of: "entity", "class", "defaultPlayer", or "game"
               Meta key (std::string)
               Meta value (std::string)
               Current line number in the source being parsed (int)
               Entity or entity class name (std::string)
                  . Do not pass if target type is "game" or "defaultPlayer"

            Output:
               ASTOperationNode
         */
         std::shared_ptr<ASTOperationNode> ASTSetMeta(std::string targetType,
         std::string metaKey, std::string metaValue, int lineNumber = 0,
         std::string entityOrClassName = "");

         /*
            Returns an AST subtree representing a setAttribute operation.

            Input:
               Target type: the kind of thing we're setting a property on (std::string)
                  . One of: "entity", "class", or "game"
               Attribute name (std::string)
               Attribute value (std::string)
               Current line number in the source being parsed (int)
               Entity or entity class name (std::string)
                  . Do not pass if target type is "game"

            Output:
               ASTOperationNode
         */
         std::shared_ptr<ASTOperationNode> ASTSetAttribute(std::string targetType,
         std::string attribute, std::string value, int lineNumber = 0,
         std::string entityOrClassName = "");

         /*
            Returns an AST subtree representing a setProperty operation.

            Input:
               Target type: the kind of thing we're setting a property on (std::string)
                  . One of: "entity", "class", or "game"
               Property name (std::string)
               Property value (std::string)
               Current line number in the source being parsed (int)
               Entity or entity class name (std::string)
                  . Do not pass if target type is "game"

            Output:
               ASTOperationNode
         */
         std::shared_ptr<ASTOperationNode> ASTSetProperty(std::string targetType,
         std::string property, std::string value, int lineNumber = 0,
         std::string entityOrClassName = "");

         /*
            Returns an AST subtree representing an insertIntoInventory operation.

            Please note that inserting an item into the default player's
            inventory is not allowed, since multiple players can be cloned from
            defaultPlayer and an Object can only be in one inventory at a time.
            The same reasoning also makes it illegal to place an Object into
            a Being class's inventory.

            Input:
               Object name (std::string)
               Being name (std::string) -- classes not allowed
               Current line number in the source being parsed (int)

            Output:
               ASTOperationNode
         */
         std::shared_ptr<ASTOperationNode> ASTInsertIntoInventory(std::string objectName,
         std::string beingName, int lineNumber = 0);

         /*
            Returns an AST subtree representing an insertIntoRoom operation.

            Input:
               Object, Player, or Creature name (std::string) -- classes not allowed
               Room name (std::string)
               Current line number in the source being parsed (int)

            Output:
               ASTOperationNode
         */
         std::shared_ptr<ASTOperationNode> ASTInsertIntoRoom(std::string thing,
         std::string room, int lineNumber = 0);

         /*
            Returns an AST subtree representing a connectRooms operation.

            Input:
               Target type: the kind of thing we're setting the connection on (std::string)
                  . One of: "entity" or "class"
               Source room (std::string)
               Room source connects to (std::string)
               Direction where connection should be made (std::string)
               Current line number in the source being parsed (int)

            Output:
               ASTOperationNode
         */
         std::shared_ptr<ASTOperationNode> ASTConnectRooms(std::string targetType,
         std::string sourceRoomOrClass, std::string connectTo, std::string direction,
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
            Parses a game definition and passes the dataa off to an instance of
            Instantiator, which creates the corresponding data structures.

            Input:
               Filename where the game definition is saved (std::string)

            Output:
               (none)
         */
         virtual void parse(std::string filename) = 0;
   };
}


#endif
