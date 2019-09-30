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
   };
}


#endif

