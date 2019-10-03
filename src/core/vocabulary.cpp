#define VOCABULARY_CPP


#include <string>
#include <unordered_set>

#include "include/vocabulary.h"
#include "include/action.h"

using namespace std;

namespace trogdor {


   Vocabulary::Vocabulary() {

      initBuiltinDirections();
      initBuiltinFillerWords();
      initBuiltinPrepositions();
   }

   /**************************************************************************/

   // Advanced stuff happens here...
   Vocabulary::~Vocabulary() {}

   /**************************************************************************/

   void Vocabulary::insertVerbAction(string verb, std::unique_ptr<Action> action) {

      verbActions[verb] = std::move(action);
   }

   /**************************************************************************/

   void Vocabulary::removeVerbAction(string verb) {

      verbActions.erase(verb);
   }

   /**************************************************************************/

   Action *Vocabulary::getVerbAction(string verb) const {

      if (verbActions.find(verb) == verbActions.end()) {

         // If the verb isn't found, see if it can be mapped via synonym
         if (
            verbSynonyms.find(verb) != verbSynonyms.end() &&
            verbActions.find(verbSynonyms.find(verb)->second) != verbActions.end()
         ) {
            return verbActions.find(verbSynonyms.find(verb)->second)->second.get();
         }

         // We got nuttin' :(
         return nullptr;
      }

      return verbActions.find(verb)->second.get();
   }

   /**************************************************************************/

   void Vocabulary::initBuiltinDirections() {

      directions.insert("north");
      directions.insert("south");
      directions.insert("east");
      directions.insert("west");
      directions.insert("northeast");
      directions.insert("northwest");
      directions.insert("southeast");
      directions.insert("southwest");
      directions.insert("up");
      directions.insert("down");
      directions.insert("inside");
      directions.insert("outside");
   }

   /**************************************************************************/

   void Vocabulary::initBuiltinFillerWords() {

      fillerWords.insert("the");
   }

   /**************************************************************************/

   void Vocabulary::initBuiltinPrepositions() {

      prepositions.insert("about");
      prepositions.insert("after");
      prepositions.insert("against");
      prepositions.insert("along");
      prepositions.insert("alongside");
      prepositions.insert("amid");
      prepositions.insert("amidst");
      prepositions.insert("among");
      prepositions.insert("amongst");
      prepositions.insert("around");
      prepositions.insert("aside");
      prepositions.insert("astride");
      prepositions.insert("at");
      prepositions.insert("before");
      prepositions.insert("behind");
      prepositions.insert("below");
      prepositions.insert("beneath");
      prepositions.insert("beside");
      prepositions.insert("besides");
      prepositions.insert("between");
      prepositions.insert("beyond");
      prepositions.insert("by");
      prepositions.insert("for");
      prepositions.insert("from");
      prepositions.insert("in");
      prepositions.insert("inside");
      prepositions.insert("into");
      prepositions.insert("near");
      prepositions.insert("of");
      prepositions.insert("off");
      prepositions.insert("on");
      prepositions.insert("onto");
      prepositions.insert("out");
      prepositions.insert("over");
      prepositions.insert("through");
      prepositions.insert("to");
      prepositions.insert("toward");
      prepositions.insert("towards");
      prepositions.insert("under");
      prepositions.insert("underneath");
      prepositions.insert("upon");
      prepositions.insert("via");
      prepositions.insert("with");
      prepositions.insert("within");
      prepositions.insert("without");
   }
}

