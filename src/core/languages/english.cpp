#include <string>
#include "../include/languages/english.h"

using namespace std;

namespace trogdor {


   string English::pluralizeNoun(string noun) const {

      // If we can't find a more specific pluralization rule, just append 's'
      // and call it a day!
      return noun + 's';
   }
}

