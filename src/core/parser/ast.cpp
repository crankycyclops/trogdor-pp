#include <string>
#include <iostream>
#include <typeinfo>

#include "../include/parser/ast.h"

using namespace std;

namespace trogdor {


   void ASTNode::printNode(string tab) const {

      cout << tab << "Value: " << (getValue().length() < 80 ? getValue() :
         getValue().substr(0, 77) + "...") << endl;
      cout << tab << "Type: " << ASTNode::getTypeStr(getType()) << endl;
      cout << tab << "Line: " << getLineNumber() << endl;
      cout << tab << "Children: " << to_string(size()) << endl;
      cout << endl;
   }

   /**************************************************************************/

   void ASTOperationNode::printNode(string tab) const {

      cout << tab << "Operation: "
         << ASTOperationNode::getOperationStr(operation) << endl;
      cout << tab << "Line: " << getLineNumber() << endl;
      cout << tab << "Children: " << to_string(size()) << endl;
      cout << endl;
   }
}

