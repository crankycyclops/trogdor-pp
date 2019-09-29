#ifndef VOCABULARY_H
#define VOCABULARY_H


#include <string>
#include <unordered_set>

using namespace std;

namespace trogdor {


   class Vocabulary {

      private:

         // Built-in directions
         unordered_set<string> directions;

         // Words that we ignore during parsing
         unordered_set<string> fillerWords;

         // Built-in prepositions
         unordered_set<string> prepositions;

      public:

         /*
            Constructor
         */
         Vocabulary();

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
            Inserts a new preposition.

            Input:
               New preposition (string)

            Output:
               (none)
         */
         inline void insertPreposition(string prep) {

            prepositions.insert(prep);
         }
   };
}


#endif

