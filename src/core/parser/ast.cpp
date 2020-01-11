#include <string>
#include <iostream>
#include <typeinfo>

#include <trogdor/parser/ast.h>

namespace trogdor {


   void ASTNode::printNode(std::string tab) const {

      std::cout << tab << "Value: " << (getValue().length() < 80 ? getValue() :
         getValue().substr(0, 77) + "...") << std::endl;
      std::cout << tab << "Type: " << ASTNode::getTypeStr(getType()) << std::endl;
      std::cout << tab << "Line: " << getLineNumber() << std::endl;
      std::cout << tab << "Children: " << std::to_string(size()) << std::endl;
      std::cout << std::endl;
   }

   /**************************************************************************/

   void ASTOperationNode::printNode(std::string tab) const {

      std::cout << tab << "Operation: "
         << ASTOperationNode::getOperationStr(operation) << std::endl;
      std::cout << tab << "Line: " << getLineNumber() << std::endl;
      std::cout << tab << "Children: " << std::to_string(size()) << std::endl;
      std::cout << std::endl;
   }
}
