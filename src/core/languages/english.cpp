#include <string>
#include "../include/languages/english.h"

using namespace std;

namespace trogdor {


   English::English() {

      // One-off exceptions to the rules
      pluralNounExceptions["foot"] = "feet";
      pluralNounExceptions["tooth"] = "teeth";
      pluralNounExceptions["goose"] = "geese";
      pluralNounExceptions["mongoose"] = "mongooses";
      pluralNounExceptions["die"] = "dice";
      pluralNounExceptions["ox"] = "oxen";
      pluralNounExceptions["child"] = "children";
      pluralNounExceptions["varmint"] = "vermin";
      pluralNounExceptions["beef"] = "beefs";
      pluralNounExceptions["brother"] = "brothers";
      pluralNounExceptions["cow"] = "cows";
      pluralNounExceptions["genie"] = "genies";
      pluralNounExceptions["money"] = "moneys";
      pluralNounExceptions["mythos"] = "mythoi";
      pluralNounExceptions["octopus"] = "octopuses";
      pluralNounExceptions["soliloquy"] = "soliloquies";
      pluralNounExceptions["trilby"] = "trilbys";
   }

   /**************************************************************************/

   string English::pluralizeNoun(string noun) const {

      // First, we try to match the noun against a list of one-off exceptions
      auto exception = pluralNounExceptions.find(noun);

      if (pluralNounExceptions.end() != exception) {
         return exception->second;
      }

      // If we can't find a more specific pluralization rule, just append 's'
      // and call it a day!
      else {
         return noun + 's';
      }
   }
}

