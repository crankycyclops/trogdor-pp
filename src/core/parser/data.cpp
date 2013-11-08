#include <string>
#include <cstring>

using namespace std;


const char *g_reservedClassNames[] = {
   "entity",
   "object",
   "creature",
   "room",
   "player",
   0
};

/******************************************************************************/

bool isClassNameReserved(string name) {

   for (int i = 0; g_reservedClassNames[i] != 0; i++) {
      if (0 == strcmp(name.c_str(), g_reservedClassNames[i])) {
         return true;
      }
   }

   return false;
}

