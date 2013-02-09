using namespace std;

#include "include/command.h"

namespace core {

   Command::Command() {

      // command is in an error state until we finish parsing
      error = false;
   }
}

