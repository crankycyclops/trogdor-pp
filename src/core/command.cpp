#include "include/command.h"

using namespace std;

namespace core {

   Command::Command() {

      // command is in an error state until we finish parsing
      error = false;
   }
}

