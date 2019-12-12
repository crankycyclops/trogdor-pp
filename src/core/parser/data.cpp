#include <string>
#include <cstring>


const char *g_reservedClassNames[] = {
   "entity",
   "place",
   "room",
   "thing",
   "object",
   "being",
   "player",
   "creature",
   0
};

/******************************************************************************/

bool isClassNameReserved(std::string name) {

   for (int i = 0; g_reservedClassNames[i] != 0; i++) {
      if (0 == strcmp(name.c_str(), g_reservedClassNames[i])) {
         return true;
      }
   }

   return false;
}
