#include "include/actionmap.h"

using namespace std;

namespace core {


   Action *ActionMap::getAction(string verb) const {

      if (actionTable.find(verb) == actionTable.end()) {
         return 0;
      }

      return actionTable.find(verb)->second;
   }
}
