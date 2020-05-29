#define VOCABULARY_CPP


#include <string>
#include <unordered_set>

#include <trogdor/vocabulary.h>
#include <trogdor/action.h>
#include <trogdor/actions.h>

namespace trogdor {


   Vocabulary::Vocabulary() {

      initBuiltinDirections();
      initBuiltinFillerWords();
      initBuiltinPrepositions();
      initBuiltinVerbs();
   }

   /**************************************************************************/

   // Clearly, advanced stuff happens here...
   Vocabulary::~Vocabulary() {}

   /**************************************************************************/

   void Vocabulary::insertVerbAction(std::string verb, std::unique_ptr<Action> action) {

      verbActions[verb] = std::move(action);
   }

   /**************************************************************************/

   void Vocabulary::removeVerbAction(std::string verb) {

      verbActions.erase(verb);
   }

   /**************************************************************************/

   Action *Vocabulary::getVerbAction(std::string verb) const {

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

      directionSynonyms["n"]   = "north";
      directionSynonyms["s"]   = "south";
      directionSynonyms["e"]   = "east";
      directionSynonyms["w"]   = "west";
      directionSynonyms["ne"]  = "northeast";
      directionSynonyms["nw"]  = "northwest";
      directionSynonyms["se"]  = "southeast";
      directionSynonyms["sw"]  = "southwest";
      directionSynonyms["in"]  = "inside";
      directionSynonyms["out"] = "outside";
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

   /**************************************************************************/

   void Vocabulary::initBuiltinVerbs() {

      insertVerbAction("fuck", std::make_unique<CussAction>());

      insertVerbSynonym("shit", "fuck");
      insertVerbSynonym("bitch", "fuck");
      insertVerbSynonym("damn", "fuck");
      insertVerbSynonym("damnit", "fuck");
      insertVerbSynonym("asshole", "fuck");
      insertVerbSynonym("asshat", "fuck");

      insertVerbAction("inv", std::make_unique<InventoryAction>());

      insertVerbSynonym("inventory", "inv");
      insertVerbSynonym("list", "inv");

      insertVerbAction("move", std::make_unique<MoveAction>());

      insertVerbSynonym("go", "move");
      insertVerbSynonym("walk", "move");
      for (auto dIt = directions.begin(); dIt != directions.end(); dIt++) {
         insertVerbSynonym(*dIt, "move");
      }
      for (auto dSynIt = directionSynonyms.begin();
      dSynIt != directionSynonyms.end(); dSynIt++) {
         insertVerbSynonym(dSynIt->first, "move");
      }

      insertVerbAction("look", std::make_unique<LookAction>());

      insertVerbSynonym("observe", "look");
      insertVerbSynonym("see", "look");
      insertVerbSynonym("show", "look");
      insertVerbSynonym("describe", "look");
      insertVerbSynonym("examine", "look");

      insertVerbAction("read", std::make_unique<ReadAction>());

      insertVerbAction("take", std::make_unique<TakeAction>());

      insertVerbSynonym("acquire", "take");
      insertVerbSynonym("get", "take");
      insertVerbSynonym("grab", "take");
      insertVerbSynonym("own", "take");
      insertVerbSynonym("claim", "take");
      insertVerbSynonym("carry", "take");

      insertVerbAction("drop", std::make_unique<DropAction>());

      insertVerbAction("attack", std::make_unique<AttackAction>());

      insertVerbSynonym("hit", "attack");
      insertVerbSynonym("harm", "attack");
      insertVerbSynonym("kill", "attack");
      insertVerbSynonym("injure", "attack");
      insertVerbSynonym("maim", "attack");
      insertVerbSynonym("fight", "attack");

      // This is a special verb that doesn't actually have an action associated
      // with it, but which instructs the game to re-execute the last command
      insertVerbAction("again", nullptr);
      insertVerbSynonym("a", "again");
   }
}

