#ifndef VOCABULARY_H
#define VOCABULARY_H


#include <string>
#include <memory>
#include <unordered_set>
#include <unordered_map>

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

            verbSynonyms[synonym] = verb;
         }
   };
}


#endif

