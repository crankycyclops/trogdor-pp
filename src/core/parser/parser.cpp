#include "../include/parser/parser.h"

namespace trogdor {


   std::shared_ptr<ASTOperationNode> Parser::ASTDefineDirection(string direction,
   int lineNumber) {

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

   std::shared_ptr<ASTOperationNode> Parser::ASTDefineDirectionSynonym(string direction,
   string synonym, int lineNumber) {

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

   std::shared_ptr<ASTOperationNode> Parser::ASTDefineVerbSynonym(string verb,
   string synonym, int lineNumber) {

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

   std::shared_ptr<ASTOperationNode> Parser::ASTDefineEntityClass(string className,
   string entityType, int lineNumber) {

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

   std::shared_ptr<ASTOperationNode> Parser::ASTDefineEntity(string entityName,
   string entityType, string className, int lineNumber) {

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

      return operation;
   }

   /**************************************************************************/

   // TODO: validate targetType and throw UndefinedException if necessary
   std::shared_ptr<ASTOperationNode> Parser::ASTSetMessage(string targetType,
   string messageName, string message, int lineNumber, string entityOrClassName) {

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
   std::shared_ptr<ASTOperationNode> Parser::ASTSetTag(string targetType,
   string tag, int lineNumber, string entityOrClassName) {

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
   std::shared_ptr<ASTOperationNode> Parser::ASTRemoveTag(string targetType,
   string tag, int lineNumber, string entityOrClassName) {

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
   std::shared_ptr<ASTOperationNode> Parser::ASTLoadScript(string targetType,
   string scriptMode, string script, int lineNumber, string entityOrClassName) {

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
   std::shared_ptr<ASTOperationNode> Parser::ASTSetEvent(string targetType,
   string eventName, string luaFunction, int lineNumber, string entityOrClassName) {

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

   std::shared_ptr<ASTOperationNode> Parser::ASTSetAlias(string targetType,
   string alias, string thingOrClassName, int lineNumber) {

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
   std::shared_ptr<ASTOperationNode> Parser::ASTSetMeta(string targetType,
   string metaKey, string metaValue, int lineNumber, string entityOrClassName) {

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
   std::shared_ptr<ASTOperationNode> Parser::ASTSetAttribute(string targetType,
   string attribute, string value, int lineNumber, string entityOrClassName) {

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
   std::shared_ptr<ASTOperationNode> Parser::ASTSetProperty(string targetType,
   string property, string value, int lineNumber, string entityOrClassName) {

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
   std::shared_ptr<ASTOperationNode> Parser::ASTInsertIntoInventory(string objectName,
   string beingName, int lineNumber) {

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

   std::shared_ptr<ASTOperationNode> Parser::ASTInsertIntoRoom(string thing,
   string room, int lineNumber) {

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

   std::shared_ptr<ASTOperationNode> Parser::ASTConnectRooms(string targetType,
   string sourceRoomOrClass, string connectTo, string direction,
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
}

