#include "include/command.h"

using namespace std;

namespace core {


   Command::Command() {

      // command is invalid until we successfully parse a command
      invalid = true;
   }


   void Command::execute() {
      // TODO: print error message if error == true
   }
}

