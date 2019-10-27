#ifndef VOCABULARY_H
#define VOCABULARY_H


#include <string>
#include <memory>
#include <unordered_set>
#include <unordered_map>

#include "exception/validationexception.h"

using namespace std;

namespace trogdor {


   // Forward declare Action so I don't have to include action.h, which also
   // includes vocabulary.h
   class Action;

   class Vocabulary {

      private:

         // Built-in directions
         unordered_set<string> directions;

         // Words that we ignore during parsing
         unordered_set<string> fillerWords;

         // Built-in prepositions
         unordered_set<string> prepositions;

         // Maps verbs to Actions
         unordered_map<string, std::unique_ptr<Action>> verbActions;

         // Maps direction synonyms to actual directions
         unordered_map<string, string> directionSynonyms;

         // Maps verb synonyms to verbs with actions
         unordered_map<string, string> verbSynonyms;

         /*
            Setup filler words that the engine recognizes and skips over during
            command parsing by default.

            Input:
               (none)

            Output:
               (none)
         */
         void initBuiltinFillerWords();

         /*
            Setup built-in directions that the engine recognizes by default.

            Input:
               (none)

            Output:
               (none)
         */
         void initBuiltinDirections();

         /*
            Setup prepositions that the engine recognizes by default.

            Input:
               (none)

            Output:
               (none)
         */
         void initBuiltinPrepositions();

         /*
            Sets up verb actions that the engine recognizes by default.

            Input:
               (none)

            Output:
               (none)
         */
         void initBuiltinVerbs();

      public:

         /*
            Constructor
         */
         Vocabulary();

         /*
            Empty destructor that satisfies Ye Olde Compiler Gods (otherwise, I
            get this error: unique_ptr.h: error: invalid application of 'sizeof'
            to incomplete type 'trogdor::Action.'
         */
         ~Vocabulary();

         /*
            Don't allow copying Vocabulary objects (there's no point, and there
            should only be one)
         */
         Vocabulary& operator=(const Vocabulary &) = delete;
         Vocabulary(const Vocabulary &) = delete;

         /*
            Returns true if the specified word is a valid direction and false if
            it's not.

            Input:
               string

            Output:
               bool
         */
         inline bool isDirection(string str) const {

            return directions.find(str) != directions.end() ? true : false;
         }

         /*
            Returns true if the specified word is a direction synonym and false
            if it's not.

            Input:
               string

            Output:
               bool
         */
         inline bool isDirectionSynonym(string str) const {

            return directionSynonyms.find(str) != directionSynonyms.end() ? true : false;
         }

         /*
            Returns a const_iterator that can be used to iterate through all
            currently defined directions.

            Input:
               (none)

            Output:
               const_iterator
         */
         inline unordered_set<string>::const_iterator directionsBegin() const {

            return directions.begin();
         }

         /*
            Returns a const_iterator representing the end of all currently
            defined directions.

            Input:
               (none)

            Output:
               const_iterator
         */
         inline unordered_set<string>::const_iterator directionsEnd() const {

            return directions.end();
         }

         /*
            Inserts a new valid direction.

            Input:
               New direction (string)

            Output:
               (none)
         */
         inline void insertDirection(string dir) {

            directions.insert(dir);
            insertVerbSynonym(dir, "move");
         }

         /*
            Inserts a new direction synonym.

            Input:
               Synonym (string)
               Original direction name (string)

            Output:
               (none)
         */
         inline void insertDirectionSynonym(string synonym, string direction) {

            if (directions.find(direction) == directions.end()) {
               throw ValidationException(string("Direction '") + direction + "' does not exist");
            }

            directionSynonyms[synonym] = direction;
            insertVerbSynonym(synonym, "move");
         }

         /*
            Returns the direction referenced either by its name or, if a
            direction by that name doesn't exist, the direction referenced by
            the synonym. If neither references a valid direction, an empty
            string is returned.

            Input:
               Direction or Direction synonym (string)

            Output:
               Direction if one exists or an empty string if not (string)
         */
         inline string getDirection(string dirOrSyn) const {

            if (directions.find(dirOrSyn) != directions.end()) {
               return dirOrSyn;
            }

            else if (directionSynonyms.find(dirOrSyn) != directionSynonyms.end()) {
               return directionSynonyms.find(dirOrSyn)->second;
            }

            else {
               return "";
            }
         }

         /*
            Returns true if the specified word is a filler word and false if
            it's not.

            Input:
               string

            Output:
               bool
         */
         inline bool isFillerWord(string str) const {

            return fillerWords.find(str) != fillerWords.end() ? true : false;
         }

         /*
            Returns a const_iterator that can be used to iterate through all
            currently defined filler words.

            Input:
               (none)

            Output:
               const_iterator
         */
         inline unordered_set<string>::const_iterator fillerWordsBegin() const {

            return fillerWords.begin();
         }

         /*
            Returns a const_iterator representing the end of all currently
            defined filler words.

            Input:
               (none)

            Output:
               const_iterator
         */
         inline unordered_set<string>::const_iterator fillerWordsEnd() const {

            return fillerWords.end();
         }

         /*
            Inserts a new filler word.

            Input:
               New filler word (string)

            Output:
               (none)
         */
         inline void insertFillerWord(string filler) {

            fillerWords.insert(filler);
         }

         /*
            Returns true if the specified word is a preposition and false if
            it's not.

            Input:
               string

            Output:
               bool
         */
         inline bool isPreposition(string str) const {

            return prepositions.find(str) != prepositions.end() ? true : false;
         }

         /*
            Returns a const_iterator that can be used to iterate through all
            currently defined prepositions.

            Input:
               (none)

            Output:
               const_iterator
         */
         inline unordered_set<string>::const_iterator prepositionsBegin() const {

            return prepositions.begin();
         }

         /*
            Returns a const_iterator representing the end of all currently
            defined prepositions.

            Input:
               (none)

            Output:
               const_iterator
         */
         inline unordered_set<string>::const_iterator prepositionsEnd() const {

            return prepositions.end();
         }

         /*
            Inserts a new preposition.

            Input:
               New preposition (string)

            Output:
               (none)
         */
         inline void insertPreposition(string prep) {

            prepositions.insert(prep);
         }

         /*
            Returns true if the specified word is a verb or a verb synonym and
            false if it's not.

            Input:
               string

            Output:
               bool
         */
         inline bool isVerb(string str) const {

            bool found = false;

            if (verbActions.find(str) != verbActions.end()) {
               found = true;
            }

            else if (
               verbSynonyms.find(str) != verbSynonyms.end() &&
               verbActions.find(verbSynonyms.find(str)->second) != verbActions.end()
            ) {
               found = true;
            }

            return found;
         }

         /*
            Returns true if the specified word is a verb synonym and false if
            it's an actual verb or not defined at all.

            Input:
               string

            Output:
               bool
         */
         inline bool isVerbSynonym(string str) const {

            return verbSynonyms.end() != verbSynonyms.find(str) ? true : false;
         }

         /*
            Inserts a new Action object identified by the specified verb.

            Input:
               Verb (string)
               Action to execute when the verb is used as a command (Action)

            Output:
               (none)
         */
         void insertVerbAction(string verb, std::unique_ptr<Action> action);

         /*
            Removes the specified verb and its corresponding Action from the
            verbActions table so that it can no longer be used in the game.

            Input:
               Verb (string)

            Output:
               (none)
         */
         void removeVerbAction(string verb);

         /*
            Retrieves the Action object identified by the specified verb. If the
            Action is not found, it next tries to locate the appropriate verb
            using a verb synonym. If a suitable verb with action still isn't
            found, nullptr is returned.

            Note that actual verbs take precedence over synonyms in situations
            where a synonym also matches a verb action.

            Input:
               Verb (string)

            Output:
               Pointer to the Action object or nullptr (Action *)
         */
         Action *getVerbAction(string verb) const;

         /*
            Sets a synonym => verb association.

            Input:
               Synonym (string)
               Verb (string)

            Output:
               (none)
         */
         inline void insertVerbSynonym(string synonym, string verb) {

            if (verbActions.find(verb) == verbActions.end()) {
               throw ValidationException(string("Verb '") + verb + "' does not exist");
            }

            verbSynonyms[synonym] = verb;
         }
   };
}


#endif

