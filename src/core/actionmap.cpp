#include "include/actionmap.h"

using namespace std;

namespace trogdor {


   Action *ActionMap::getAction(string verb) const {

      if (actionTable.find(verb) == actionTable.end()) {
         return nullptr;
      }

      return actionTable.find(verb)->second.get();
   }
}

