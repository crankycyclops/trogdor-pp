#ifndef AST_H
#define AST_H


#include <memory>
#include <vector>
#include <string>
#include <ostream>


namespace trogdor {


   // Node types
   enum ASTNodeType {
      AST_ROOT = 0,
      AST_OPERATION = 1,
      AST_VALUE = 2
   };

   // List of valid operations for nodes of AST_OPERATION type
   enum ASTOperation {
      DEFINE_DIRECTION = 0,
      DEFINE_DIRECTION_SYNONYM = 1,
      DEFINE_VERB_SYNONYM = 2,
      DEFINE_ENTITY = 3,
      DEFINE_ENTITY_CLASS = 4,
      SET_MESSAGE = 5,
      SET_TAG = 6,
      REMOVE_TAG = 7,
      LOAD_SCRIPT = 8,
      SET_EVENT = 9,
      SET_ALIAS = 10,
      SET_META = 11,
      SET_ATTRIBUTE = 12,
      SET_PROPERTY = 13,
      INSERT_INTO_INVENTORY = 14,
      INSERT_INTO_PLACE = 15,
      CONNECT_ROOMS = 16
   };

   /**************************************************************************/

   /*
      Abstract syntax tree that defines a game.
   */
   class ASTNode {

      private:

         std::string value;
         ASTNodeType type;
         int lineno;

         std::vector<std::shared_ptr<ASTNode>> children;

      public:

         /*
            Returns a string representation of the node's type.

            Input:
               Node type (ASTNodeType)

            Output:
               String version of the type (std::string)
         */
         static std::string getTypeStr(ASTNodeType type) {

            switch (type) {

               case AST_ROOT:
                  return "AST_ROOT";

               case AST_OPERATION:
                  return "AST_OPERATION";

               case AST_VALUE:
                  return "AST_VALUE";

               default:
                  return "UNDEFINED";
            }
         }

         /*
            Constructors
         */
         inline ASTNode(): value(""), type(AST_ROOT), lineno(0) {}
         inline ASTNode(std::string v, ASTNodeType t, int l = 0): value(v), type(t), lineno(l) {}

         // Required for dynamic_cast to work
         virtual ~ASTNode() = default;

         /*
            Get the node's value.

            Input:
               (none)

            Output:
               Value (std::string)
         */
         inline std::string getValue() const {return value;}

         /*
            Get the node's type.

            Input:
               (none)

            Output:
               Node Type (ASTNodeType)
         */
         inline ASTNodeType getType() const {return type;}

         /*
            Get the line number associated with the current node.

            Input:
               (none)

            Output:
               Line number (int)
         */
         inline int getLineNumber() const {return lineno;}

         /*
            Get the tree's or subtree's size in terms of number of children.

            Input:
               (none)

            Output:
               Tree or substree size (int)
         */
         inline std::vector<std::shared_ptr<ASTNode>>::size_type size() const {

            return children.size();
         }

         /*
            Return read-only reference to the tree's or subtree's children.

            Input:
               (none)

            Output:
               const std::vector<ASTNode> &
         */
         inline const std::vector<std::shared_ptr<ASTNode>> &getChildren() const {

            return children;
         }

         /*
            Append a child node.

            Input:
               Child node (ASTNode)

            Output:
               (none)
         */
         inline void appendChild(std::shared_ptr<ASTNode> child) {

            children.push_back(child);
         }

         /*
            Outputs a string representation of the AST tree or subtree for
            debugging purposes.

            Input:
               Tab space (std::string)

            Output:
               (none)
         */
         virtual void printNode(std::string tab = "") const;
   };

   /**************************************************************************/

   /*
      Identifies a specific operation in the AST
   */
   class ASTOperationNode: public ASTNode {

      private:

         // Identifies the specific operation the node represents
         ASTOperation operation;

      public:

         /*
            Returns a string representation of the node's operation.

            Input:
               Operation (ASTOperation)

            Output:
               String version of the operation (std::string)
         */
         static std::string getOperationStr(ASTOperation operation) {

            switch (operation) {

               case DEFINE_DIRECTION:
                  return "DEFINE_DIRECTION";

               case DEFINE_DIRECTION_SYNONYM:
                  return "DEFINE_DIRECTION_SYNONYM";

               case DEFINE_VERB_SYNONYM:
                  return "DEFINE_VERB_SYNONYM";

               case DEFINE_ENTITY:
                  return "DEFINE_ENTITY";

               case DEFINE_ENTITY_CLASS:
                  return "DEFINE_ENTITY_CLASS";

               case SET_MESSAGE:
                  return "SET_MESSAGE";

               case SET_TAG:
                  return "SET_TAG";

               case REMOVE_TAG:
                  return "REMOVE_TAG";

               case LOAD_SCRIPT:
                  return "LOAD_SCRIPT";

               case SET_EVENT:
                  return "SET_EVENT";

               case SET_ALIAS:
                  return "SET_ALIAS";

               case SET_META:
                  return "SET_META";

               case SET_ATTRIBUTE:
                  return "SET_ATTRIBUTE";

               case SET_PROPERTY:
                  return "SET_PROPERTY";

               case INSERT_INTO_INVENTORY:
                  return "INSERT_INTO_INVENTORY";

               case INSERT_INTO_PLACE:
                  return "INSERT_INTO_PLACE";

               case CONNECT_ROOMS:
                  return "CONNECT_ROOMS";

               default:
                  return "UNDEFINED";
            }
         }

         /*
            Constructors
         */
         inline ASTOperationNode(ASTOperation o, int l = 0):
            ASTNode("", AST_OPERATION, l), operation(o) {}
         ASTOperationNode() = delete;

         /*
            Returns the operation represented by the ASTNode.

            Input:
               (none)

            Output:
               Operation (ASTOperation)
         */
         ASTOperation getOperation() const {return operation;}

         /*
            Outputs an operation node for debugging purposes.

            Input:
               Tab space (std::string)

            Output:
               (none)
         */
         virtual void printNode(std::string tab = "") const;
   };

   /**************************************************************************/

   /*
      Recursively prints an entire AST. Since I don't forsee the AST getting too
      deep, I think a recursive implementation is okay. If that changes, I'll
      refactor this.

      Input:
         Child node (std::shared_ptr<ASTNode> &)
         Amount of tab space to print in front of the node's data (std::string)

      Output:
         (none)
   */
   inline void printAST(const std::shared_ptr<ASTNode> &node, std::string tab = "") {

      node->printNode(tab);

      for (const auto &child: node->getChildren()) {
         printAST(child, tab + "\t");
      }
   }

   /**************************************************************************/

   /*
      Wrapper around printAST() that makes it compatible with cout and other
      ostream objects.

      Input:
         Reference to output stream (ostream &)
         Pointer to child node (std::shared_ptr<ASTNode> &)

      Output:
         (none)
   */
   inline std::ostream &operator<<(std::ostream &os, const std::shared_ptr<ASTNode> &node) {

      printAST(node);
      return os;
   };
}


#endif
