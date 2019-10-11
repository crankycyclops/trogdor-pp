#include <string>
#include <regex>

#include "../include/languages/english.h"

using namespace std;

namespace trogdor {


   English::English() {

      // One-off exceptions to the rules
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

      // Most nouns ending in -a get an 's' stuck at the end, but these retain
      // their original latin plural endings
      pluralNounExceptions["alumna"] = "alumnae";
      pluralNounExceptions["alga"] = "algae";
      pluralNounExceptions["vertebra"] = "vertebrae";

      // Most nouns ending in -ex get an "es" stuck at the end, but these retain
      // their original latin plural endings
      pluralNounExceptions["codex"] = "codices";
      pluralNounExceptions["murex"] = "murices";
      pluralNounExceptions["silex"] = "silices";

      // Most nouns ending in -is get an "es" stuck at the end, but these retain
      // their original latin plural endings
      pluralNounExceptions["iris"] = "irises";
      pluralNounExceptions["clitoris"] = "clitorises";
      pluralNounExceptions["penis"] = "penises";

      // Most nouns ending in -o and not preceded by a vowel end in oes.
      // However, there are some exceptions.
      pluralNounExceptions["albino"] = "albinos";
      pluralNounExceptions["alto"] = "altos";
      pluralNounExceptions["archipelago"] = "archipelagos";
      pluralNounExceptions["armadillo"] = "armadillos";
      pluralNounExceptions["basso"] = "bassos";
      pluralNounExceptions["canto"] = "cantos";
      pluralNounExceptions["commando"] = "commandos";
      pluralNounExceptions["contralto"] = "contraltos";
      pluralNounExceptions["crescendo"] = "crescendos";
      pluralNounExceptions["ditto"] = "dittos";
      pluralNounExceptions["dynamo"] = "dynamos";
      pluralNounExceptions["embryo"] = "embryos";
      pluralNounExceptions["fiasco"] = "fiascos";
      pluralNounExceptions["generalissimo"] = "generalissimos";
      pluralNounExceptions["ghetto"] = "ghettos";
      pluralNounExceptions["guano"] = "guanos";
      pluralNounExceptions["inferno"] = "infernos";
      pluralNounExceptions["jumbo"] = "jumbos";
      pluralNounExceptions["lingo"] = "lingos";
      pluralNounExceptions["lumbago"] = "lumbagos";
      pluralNounExceptions["magneto"] = "magnetos";
      pluralNounExceptions["manifesto"] = "manifestos";
      pluralNounExceptions["medico"] = "medicos";
      pluralNounExceptions["octavo"] = "octavos";
      pluralNounExceptions["photo"] = "photos";
      pluralNounExceptions["pro"] = "pros";
      pluralNounExceptions["quarto"] = "quartos";
      pluralNounExceptions["rhino"] = "rhinos";
      pluralNounExceptions["solo"] = "solos";
      pluralNounExceptions["soprano"] = "sopranos";
      pluralNounExceptions["stylo"] = "stylos";
      pluralNounExceptions["tempo"] = "tempos";

      // Individual nouns that don't change when made plural
      pluralNounExceptions["advice"] = "advice";
      pluralNounExceptions["aircraft"] = "aircraft";
      pluralNounExceptions["alms"] = "alms";
      pluralNounExceptions["alumimum"] = "alumimum";
      pluralNounExceptions["barracks"] = "barracks";
      pluralNounExceptions["binoculars"] = "binoculars";
      pluralNounExceptions["bison"] = "bison";
      pluralNounExceptions["bream"] = "bream";
      pluralNounExceptions["breeches"] = "breeches";
      pluralNounExceptions["britches"] = "britches";
      pluralNounExceptions["buffalo"] = "buffalo";
      pluralNounExceptions["caribou"] = "caribou";
      pluralNounExceptions["carp"] = "carp";
      pluralNounExceptions["cattle"] = "cattle";
      pluralNounExceptions["chalk"] = "chalk";
      pluralNounExceptions["chassis"] = "chassis";
      pluralNounExceptions["chinos"] = "chinos";
      pluralNounExceptions["clothing"] = "clothing";
      pluralNounExceptions["cod"] = "cod";
      pluralNounExceptions["concrete"] = "concrete";
      pluralNounExceptions["contretemps"] = "contretemps";
      pluralNounExceptions["corps"] = "corps";
      pluralNounExceptions["crossroads"] = "crossroads";
      pluralNounExceptions["debris"] = "debris";
      pluralNounExceptions["diabetes"] = "diabetes";
      pluralNounExceptions["dice"] = "dice";
      pluralNounExceptions["djinn"] = "djinn";
      pluralNounExceptions["doldrums"] = "doldrums";
      pluralNounExceptions["dungarees"] = "dungarees";
      pluralNounExceptions["education"] = "education";
      pluralNounExceptions["eland"] = "eland";
      pluralNounExceptions["elk"] = "elk";
      pluralNounExceptions["eyeglasses"] = "eyeglasses";
      pluralNounExceptions["flounder"] = "flounder";
      pluralNounExceptions["flour"] = "flour";
      pluralNounExceptions["foramen"] = "foramens";
      pluralNounExceptions["furniture"] = "furniture";
      pluralNounExceptions["gallows"] = "gallows";
      pluralNounExceptions["graffiti"] = "graffiti";
      pluralNounExceptions["haddock"] = "haddock";
      pluralNounExceptions["halibut"] = "halibut";
      pluralNounExceptions["headquarters"] = "headquarters";
      pluralNounExceptions["help"] = "help";
      pluralNounExceptions["herpes"] = "herpes";
      pluralNounExceptions["highjinks"] = "highjinks";
      pluralNounExceptions["high-jinks"] = "high-jinks";
      pluralNounExceptions["homework"] = "homework";
      pluralNounExceptions["ides"] = "ides";
      pluralNounExceptions["insignia"] = "insignia";
      pluralNounExceptions["jackanapes"] = "jackanapes";
      pluralNounExceptions["jeans"] = "jeans";
      pluralNounExceptions["knickers"] = "knickers";
      pluralNounExceptions["knowledge"] = "knowledge";
      pluralNounExceptions["kudos"] = "kudos";
      pluralNounExceptions["leggins"] = "leggins";
      pluralNounExceptions["lumen"] = "lumens";
      pluralNounExceptions["mackerel"] = "mackerel";
      pluralNounExceptions["measles"] = "measles";
      pluralNounExceptions["mews"] = "mews";
      pluralNounExceptions["moose"] = "moose";
      pluralNounExceptions["mumps"] = "mumps";
      pluralNounExceptions["news"] = "news";
      pluralNounExceptions["offspring"] = "offspring";
      pluralNounExceptions["oxygen"] = "oxygen";
      pluralNounExceptions["pants"] = "pants";
      pluralNounExceptions["pincers"] = "pincers";
      pluralNounExceptions["pliers"] = "pliers";
      pluralNounExceptions["police"] = "police";
      pluralNounExceptions["premises"] = "premises";
      pluralNounExceptions["pyjamas"] = "pyjamas";
      pluralNounExceptions["rabies"] = "rabies";
      pluralNounExceptions["rendezvous"] = "rendezvous";
      pluralNounExceptions["salmon"] = "salmon";
      pluralNounExceptions["scissors"] = "scissors";
      pluralNounExceptions["series"] = "series";
      pluralNounExceptions["shambles"] = "shambles";
      pluralNounExceptions["shears"] = "shears";
      pluralNounExceptions["shorts"] = "shorts";
      pluralNounExceptions["shrimp"] = "shrimp";
      pluralNounExceptions["smithereens"] = "smithereens";
      pluralNounExceptions["species"] = "species";
      pluralNounExceptions["squid"] = "squid";
      pluralNounExceptions["stamen"] = "stamens";
      pluralNounExceptions["sugar"] = "sugar";
      pluralNounExceptions["swine"] = "swine";
      pluralNounExceptions["tongs"] = "tongs";
      pluralNounExceptions["trousers"] = "trousers";
      pluralNounExceptions["trout"] = "trout";
      pluralNounExceptions["tuna"] = "tuna";
      pluralNounExceptions["tweezers"] = "tweezers";
      pluralNounExceptions["wheat"] = "wheat";
      pluralNounExceptions["whiting"] = "whiting";
      pluralNounExceptions["wildebeest"] = "wildebeest";
      pluralNounExceptions["wood"] = "wood";
      pluralNounExceptions["woods"] = "woods";

      // Simple regex replacements that achieve proper pluralization for some
      // nouns. Be careful! The order in which these rules are inserted is
      // important!
      regexReplacements.push_back(RegexReplacement({"man$", "men"}));
      regexReplacements.push_back(RegexReplacement({"([lm])ouse$", "$1ice"}));
      regexReplacements.push_back(RegexReplacement({"tooth$", "tooth"}));
      regexReplacements.push_back(RegexReplacement({"foot$", "feet"}));
      regexReplacements.push_back(RegexReplacement({"goose$", "geese"}));
      regexReplacements.push_back(RegexReplacement({"zoon$", "zoa"}));
      regexReplacements.push_back(RegexReplacement({"([csx])is$", "$1es"}));
      regexReplacements.push_back(RegexReplacement({"([ein])x$", "$1xes"}));
      regexReplacements.push_back(RegexReplacement({"es$", "eses"}));
      regexReplacements.push_back(RegexReplacement({"is$", "es"}));
      regexReplacements.push_back(RegexReplacement({"([cs]h)$", "$1es"}));
      regexReplacements.push_back(RegexReplacement({"ss$", "sses"}));
      regexReplacements.push_back(RegexReplacement({"ss$", "sses"}));
      regexReplacements.push_back(RegexReplacement({"([aeo]l)f$", "$1ves"}));
      regexReplacements.push_back(RegexReplacement({"([^d]ea)f$", "$1ves"}));
      regexReplacements.push_back(RegexReplacement({"([nlw]i)fe$", "$1ves"}));
      regexReplacements.push_back(RegexReplacement({"arf$", "arves"}));
      regexReplacements.push_back(RegexReplacement({"(^[A-Z].*?)y$", "$1ys"}));
      regexReplacements.push_back(RegexReplacement({"([aeiou])y$", "$1ys"}));
      regexReplacements.push_back(RegexReplacement({"([^aeiou])y$", "$1ies"}));
      regexReplacements.push_back(RegexReplacement({"([^aeiou])o$", "$1oes"}));

      // Categories of regex-matchable nouns that don't change when made plural
      regexReplacements.push_back(RegexReplacement({"bait$", "bait"}));
      regexReplacements.push_back(RegexReplacement({"fish$", "fish"}));
      regexReplacements.push_back(RegexReplacement({"ois$", "ois"}));
      regexReplacements.push_back(RegexReplacement({"sheep$", "sheep"}));
      regexReplacements.push_back(RegexReplacement({"deer$", "deer"}));
      regexReplacements.push_back(RegexReplacement({"pox$", "pox"}));
      regexReplacements.push_back(RegexReplacement({"itis$", "itis"}));
      regexReplacements.push_back(RegexReplacement({"clippers$", "clippers"}));
   }

   /**************************************************************************/

   string English::pluralizeNoun(string noun) const {

      // First, we try to match the noun against a list of one-off exceptions
      auto exception = pluralNounExceptions.find(noun);

      if (pluralNounExceptions.end() != exception) {
         return exception->second;
      }

      // Next, see if the noun can be transformed according to a simple regex
      for (auto transform = regexReplacements.begin();
      regexReplacements.end() != transform; transform++) {

         regex r(transform->regex);

         if (regex_search(noun, r)) {
            return regex_replace(noun, r, transform->replacement);
         }
      }

      // Finally, if we can't find a more specific pluralization rule, just
      // append 's' and call it a day!
      return noun + 's';
   }
}

