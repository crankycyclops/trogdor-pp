#include <trogdor/parser/parser.h>

namespace trogdor {


   // See note in the comment above the destructor definition in
   // include/parser/parser.h for an explanation of why this is here.
   Parser::~Parser() {}

   /**************************************************************************/

   std::shared_ptr<ASTOperationNode> Parser::ASTDefineDirection(
   std::string direction, int lineNumber) {

      auto operation = std::make_shared<ASTOperationNode>(
         DEFINE_DIRECTION,
         lineNumber
      );

      operation->appendChild(std::make_shared<ASTNode>(
         direction,
         AST_VALUE,
         lineNumber
      ));

      return operation;
   }

   /**************************************************************************/

   std::shared_ptr<ASTOperationNode> Parser::ASTDefineDirectionSynonym(
   std::string direction, std::string synonym, int lineNumber) {

      auto operation = std::make_shared<ASTOperationNode>(
         DEFINE_DIRECTION_SYNONYM,
         lineNumber
      );

      operation->appendChild(std::make_shared<ASTNode>(
         direction,
         AST_VALUE,
         lineNumber
      ));

      operation->appendChild(std::make_shared<ASTNode>(
         synonym,
         AST_VALUE,
         lineNumber
      ));

      return operation;
   }

   /**************************************************************************/

   std::shared_ptr<ASTOperationNode> Parser::ASTDefineVerbSynonym(std::string verb,
   std::string synonym, int lineNumber) {

      auto operation = std::make_shared<ASTOperationNode>(
         DEFINE_VERB_SYNONYM,
         lineNumber
      );

      operation->appendChild(std::make_shared<ASTNode>(
         verb,
         AST_VALUE,
         lineNumber
      ));

      operation->appendChild(std::make_shared<ASTNode>(
         synonym,
         AST_VALUE,
         lineNumber
      ));

      return operation;
   }

   /**************************************************************************/

   std::shared_ptr<ASTOperationNode> Parser::ASTDefineEntityClass(
   std::string className, std::string entityType, int lineNumber) {

      auto operation = std::make_shared<ASTOperationNode>(
         DEFINE_ENTITY_CLASS,
         lineNumber
      );

      operation->appendChild(std::make_shared<ASTNode>(
         className,
         AST_VALUE,
         lineNumber
      ));

      operation->appendChild(std::make_shared<ASTNode>(
         entityType,
         AST_VALUE,
         lineNumber
      ));

      return operation;
   }

   /**************************************************************************/

   std::shared_ptr<ASTOperationNode> Parser::ASTDefineEntity(
   std::string entityName, std::string entityType, std::string className,
   std::optional<std::string> pluralName, int lineNumber) {

      auto operation = std::make_shared<ASTOperationNode>(
         DEFINE_ENTITY,
         lineNumber
      );

      operation->appendChild(std::make_shared<ASTNode>(
         entityName,
         AST_VALUE,
         lineNumber
      ));

      operation->appendChild(std::make_shared<ASTNode>(
         entityType,
         AST_VALUE,
         lineNumber
      ));

      operation->appendChild(std::make_shared<ASTNode>(
         className,
         AST_VALUE,
         lineNumber
      ));

      // Entities of the Resource type can take a fourth optional parameter to
      // specify a custom plural version of the name
      if (pluralName) {
         operation->appendChild(std::make_shared<ASTNode>(
            *pluralName,
            AST_VALUE,
            lineNumber
         ));
      }

      return operation;
   }

   /**************************************************************************/

   // TODO: validate targetType and throw UndefinedException if necessary
   std::shared_ptr<ASTOperationNode> Parser::ASTSetMessage(std::string targetType,
   std::string messageName, std::string message, int lineNumber,
   std::string entityOrClassName) {

      auto operation = std::make_shared<ASTOperationNode>(
         SET_MESSAGE,
         lineNumber
      );

      operation->appendChild(std::make_shared<ASTNode>(
         targetType,
         AST_VALUE,
         lineNumber
      ));

      operation->appendChild(std::make_shared<ASTNode>(
         messageName,
         AST_VALUE,
         lineNumber
      ));

      operation->appendChild(std::make_shared<ASTNode>(
         message,
         AST_VALUE,
         lineNumber
      ));

      if (0 != targetType.compare("defaultPlayer")) {
         operation->appendChild(std::make_shared<ASTNode>(
            entityOrClassName,
            AST_VALUE,
            lineNumber
         ));
      }

      return operation;
   }

   /**************************************************************************/

   // TODO: validate targetType and throw UndefinedException if necessary
   std::shared_ptr<ASTOperationNode> Parser::ASTSetTag(std::string targetType,
   std::string tag, int lineNumber, std::string entityOrClassName) {

      auto operation = std::make_shared<ASTOperationNode>(
         SET_TAG,
         lineNumber
      );

      operation->appendChild(std::make_shared<ASTNode>(
         targetType,
         AST_VALUE,
         lineNumber
      ));

      operation->appendChild(std::make_shared<ASTNode>(
         tag,
         AST_VALUE,
         lineNumber
      ));

      if (0 != targetType.compare("defaultPlayer")) {
         operation->appendChild(std::make_shared<ASTNode>(
            entityOrClassName,
            AST_VALUE,
            lineNumber
         ));
      }

      return operation;
   }

   /**************************************************************************/

   // TODO: validate targetType and throw UndefinedException if necessary
   std::shared_ptr<ASTOperationNode> Parser::ASTRemoveTag(std::string targetType,
   std::string tag, int lineNumber, std::string entityOrClassName) {

      auto operation = std::make_shared<ASTOperationNode>(
         REMOVE_TAG,
         lineNumber
      );

      operation->appendChild(std::make_shared<ASTNode>(
         targetType,
         AST_VALUE,
         lineNumber
      ));

      operation->appendChild(std::make_shared<ASTNode>(
         tag,
         AST_VALUE,
         lineNumber
      ));

      if (0 != targetType.compare("defaultPlayer")) {
         operation->appendChild(std::make_shared<ASTNode>(
            entityOrClassName,
            AST_VALUE,
            lineNumber
         ));
      }

      return operation;
   }

   /**************************************************************************/

   // TODO: validate targetType and throw UndefinedException if necessary
   std::shared_ptr<ASTOperationNode> Parser::ASTLoadScript(std::string targetType,
   std::string scriptMode, std::string script, int lineNumber,
   std::string entityOrClassName) {

      auto operation = std::make_shared<ASTOperationNode>(
         LOAD_SCRIPT,
         lineNumber
      );

      operation->appendChild(std::make_shared<ASTNode>(
         targetType,
         AST_VALUE,
         lineNumber
      ));

      operation->appendChild(std::make_shared<ASTNode>(
         scriptMode,
         AST_VALUE,
         lineNumber
      ));

      operation->appendChild(std::make_shared<ASTNode>(
         script,
         AST_VALUE,
         lineNumber
      ));

      if (0 != targetType.compare("game")) {
         operation->appendChild(std::make_shared<ASTNode>(
            entityOrClassName,
            AST_VALUE,
            lineNumber
         ));
      }

      return operation;
   }

   /**************************************************************************/

   // TODO: validate targetType and throw UndefinedException if necessary
   std::shared_ptr<ASTOperationNode> Parser::ASTSetEvent(std::string targetType,
   std::string eventName, std::string luaFunction, int lineNumber,
   std::string entityOrClassName) {

      auto operation = std::make_shared<ASTOperationNode>(
         SET_EVENT,
         lineNumber
      );

      operation->appendChild(std::make_shared<ASTNode>(
         targetType,
         AST_VALUE,
         lineNumber
      ));

      operation->appendChild(std::make_shared<ASTNode>(
         eventName,
         AST_VALUE,
         lineNumber
      ));

      operation->appendChild(std::make_shared<ASTNode>(
         luaFunction,
         AST_VALUE,
         lineNumber
      ));

      if (0 != targetType.compare("game")) {
         operation->appendChild(std::make_shared<ASTNode>(
            entityOrClassName,
            AST_VALUE,
            lineNumber
         ));
      }

      return operation;
   }

   /**************************************************************************/

   std::shared_ptr<ASTOperationNode> Parser::ASTSetAlias(std::string targetType,
   std::string alias, std::string thingOrClassName, int lineNumber) {

      auto operation = std::make_shared<ASTOperationNode>(
         SET_ALIAS,
         lineNumber
      );

      operation->appendChild(std::make_shared<ASTNode>(
         targetType,
         AST_VALUE,
         lineNumber
      ));

      operation->appendChild(std::make_shared<ASTNode>(
         alias,
         AST_VALUE,
         lineNumber
      ));

      operation->appendChild(std::make_shared<ASTNode>(
         thingOrClassName,
         AST_VALUE,
         lineNumber
      ));

      return operation;
   }

   /**************************************************************************/

   // TODO: validate targetType and throw UndefinedException if necessary
   std::shared_ptr<ASTOperationNode> Parser::ASTSetMeta(std::string targetType,
   std::string metaKey, std::string metaValue, int lineNumber,
   std::string entityOrClassName) {

      auto operation = std::make_shared<ASTOperationNode>(
         SET_META,
         lineNumber
      );

      operation->appendChild(std::make_shared<ASTNode>(
         targetType,
         AST_VALUE,
         lineNumber
      ));

      operation->appendChild(std::make_shared<ASTNode>(
         metaKey,
         AST_VALUE,
         lineNumber
      ));

      operation->appendChild(std::make_shared<ASTNode>(
         metaValue,
         AST_VALUE,
         lineNumber
      ));

      if (
         0 == targetType.compare("entity") ||
         0 == targetType.compare("class")
      ) {
         operation->appendChild(std::make_shared<ASTNode>(
            entityOrClassName,
            AST_VALUE,
            lineNumber
         ));
      }

      return operation;
   }

   /**************************************************************************/

   // TODO: validate targetType and throw UndefinedException if necessary
   std::shared_ptr<ASTOperationNode> Parser::ASTSetAttribute(std::string targetType,
   std::string attribute, std::string value, int lineNumber,
   std::string entityOrClassName) {

      auto operation = std::make_shared<ASTOperationNode>(
         SET_ATTRIBUTE,
         lineNumber
      );

      operation->appendChild(std::make_shared<ASTNode>(
         targetType,
         AST_VALUE,
         lineNumber
      ));

      operation->appendChild(std::make_shared<ASTNode>(
         attribute,
         AST_VALUE,
         lineNumber
      ));

      operation->appendChild(std::make_shared<ASTNode>(
         value,
         AST_VALUE,
         lineNumber
      ));

      if (
         0 == targetType.compare("entity") ||
         0 == targetType.compare("class")
      ) {
         operation->appendChild(std::make_shared<ASTNode>(
            entityOrClassName,
            AST_VALUE,
            lineNumber
         ));
      }

      return operation;
   }

   /**************************************************************************/

   // TODO: validate targetType and throw UndefinedException if necessary
   std::shared_ptr<ASTOperationNode> Parser::ASTSetProperty(std::string targetType,
   std::string property, std::string value, int lineNumber,
   std::string entityOrClassName) {

      auto operation = std::make_shared<ASTOperationNode>(
         SET_PROPERTY,
         lineNumber
      );

      operation->appendChild(std::make_shared<ASTNode>(
         targetType,
         AST_VALUE,
         lineNumber
      ));

      operation->appendChild(std::make_shared<ASTNode>(
         property,
         AST_VALUE,
         lineNumber
      ));

      operation->appendChild(std::make_shared<ASTNode>(
         value,
         AST_VALUE,
         lineNumber
      ));

      if (
         0 == targetType.compare("entity") ||
         0 == targetType.compare("class")
      ) {
         operation->appendChild(std::make_shared<ASTNode>(
            entityOrClassName,
            AST_VALUE,
            lineNumber
         ));
      }

      return operation;
   }

   /**************************************************************************/

   // TODO: semantic check to prevent us from inserting items into default
   // player's inventory or into a class (should throw an exception in these
   // cases)
   std::shared_ptr<ASTOperationNode> Parser::ASTInsertIntoInventory(
   std::string objectName, std::string beingName, int lineNumber) {

      auto operation = std::make_shared<ASTOperationNode>(
         INSERT_INTO_INVENTORY,
         lineNumber
      );

      operation->appendChild(std::make_shared<ASTNode>(
         objectName,
         AST_VALUE,
         lineNumber
      ));

      operation->appendChild(std::make_shared<ASTNode>(
         beingName,
         AST_VALUE,
         lineNumber
      ));

      return operation;
   }

   /**************************************************************************/

   std::shared_ptr<ASTOperationNode> Parser::ASTInsertIntoRoom(std::string thing,
   std::string room, int lineNumber) {

      auto operation = std::make_shared<ASTOperationNode>(
         INSERT_INTO_PLACE,
         lineNumber
      );

      operation->appendChild(std::make_shared<ASTNode>(
         thing,
         AST_VALUE,
         lineNumber
      ));

      operation->appendChild(std::make_shared<ASTNode>(
         room,
         AST_VALUE,
         lineNumber
      ));

      return operation;
   }

   /**************************************************************************/

   std::shared_ptr<ASTOperationNode> Parser::ASTConnectRooms(std::string targetType,
   std::string sourceRoomOrClass, std::string connectTo, std::string direction,
   int lineNumber) {

      auto operation = std::make_shared<ASTOperationNode>(
         CONNECT_ROOMS,
         lineNumber
      );

      operation->appendChild(std::make_shared<ASTNode>(
         targetType,
         AST_VALUE,
         lineNumber
      ));

      operation->appendChild(std::make_shared<ASTNode>(
         sourceRoomOrClass,
         AST_VALUE,
         lineNumber
      ));

      operation->appendChild(std::make_shared<ASTNode>(
         connectTo,
         AST_VALUE,
         lineNumber
      ));

      operation->appendChild(std::make_shared<ASTNode>(
         direction,
         AST_VALUE,
         lineNumber
      ));

      return operation;
   }

   /**************************************************************************/

   std::shared_ptr<ASTOperationNode> Parser::ASTAllocateResource(std::string tangible,
   std::string resource, std::string amount, int lineNumber) {

      auto operation = std::make_shared<ASTOperationNode>(
         ALLOCATE_RESOURCE,
         lineNumber
      );

      operation->appendChild(std::make_shared<ASTNode>(
         tangible,
         AST_VALUE,
         lineNumber
      ));

      operation->appendChild(std::make_shared<ASTNode>(
         resource,
         AST_VALUE,
         lineNumber
      ));

      operation->appendChild(std::make_shared<ASTNode>(
         amount,
         AST_VALUE,
         lineNumber
      ));

      return operation;
   }
}
