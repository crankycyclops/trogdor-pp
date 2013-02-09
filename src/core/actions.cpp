#include <iostream>
#include "include/actions.h"

using namespace std;

namespace core {


   /*
      Methods for the Quit action.
   */

   bool QuitAction::checkSyntax(Command *command) {

      // A valid quit command should only be one word, a verb
      if (command->getDirectObject().length() > 0 ||
      command->getIndirectObject().length() > 0) {
         return false;
      }

      return true;
   }


   void QuitAction::execute(Command *command, Game *game) {

      // TODO
      cout << "Quit action stub!\n";
   }
}

