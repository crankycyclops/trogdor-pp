#define VOCABULARY_CPP


#include <cstring>
#include <string>

using namespace std;


const char *g_directions[] = {
   "north",
   "south",
   "east",
   "west",
   "up",
   "down",
   "in",
   "out",
   0
};

const char *g_fillerWords[] = {
   "the",
   0
};

const char *g_prepositions[] = {
   "about",
   "after",
   "against",
   "along",
   "alongside",
   "amid",
   "amidst",
   "among",
   "amongst",
   "around",
   "aside",
   "astride",
   "at",
   "before",
   "behind",
   "below",
   "beneath",
   "beside",
   "besides",
   "between",
   "beyond",
   "by",
   "for",
   "from",
   "in",
   "inside",
   "into",
   "near",
   "of",
   "off",
   "on",
   "onto",
   "out",
   "over",
   "through",
   "to",
   "toward",
   "towards",
   "under",
   "underneath",
   "upon",
   "via",
   "with",
   "within",
   "without",
   0
};

/******************************************************************************/

bool isDirection(string str) {

   for (int i = 0; g_directions[i] != 0; i++) {
      if (0 == strcmp(str.c_str(), g_directions[i])) {
         return true;
      }
   }

   return false;
}

