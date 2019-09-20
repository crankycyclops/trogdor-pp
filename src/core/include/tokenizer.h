#ifndef TOKENIZER_H
#define TOKENIZER_H


#include <string>
#include <regex>
#include <vector>

using namespace std;

namespace trogdor {

   class Tokenizer {

      private:

         vector<string> tokens;        // tokens parsed from the string
         unsigned int   curTokenIndex; // current position in the tokens vector

      public:

         Tokenizer(string s);

         /*
            Returns true if we've reached the end of the string.

            Input: (none)
            Output: bool
         */
         inline bool isEnd() const {return curTokenIndex >= tokens.size() ? true : false;}

         /*
            Retrieve the current token.

            Input: (none)
            Output: current token (string)
         */
         inline string getCurToken() const {return isEnd() ? string() : tokens[curTokenIndex];}

         /*
            Advances the current token.

            Input: (none)
            Output: (none)
         */
         inline void next() {curTokenIndex++;}

         /*
            Rewinds the tokenizer to the beginning of the string.

            Input: (none)
            Output: (none)
         */
         inline void rewind() {curTokenIndex = 0;}
   };
}


#endif

