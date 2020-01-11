#ifndef TOKENIZER_H
#define TOKENIZER_H


#include <string>
#include <regex>
#include <vector>

namespace trogdor {

   class Tokenizer {

      private:

         std::vector<std::string> tokens;        // tokens parsed from the string
         unsigned int curTokenIndex;             // current position in the tokens vector

      public:

         Tokenizer(std::string s);

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
         inline std::string getCurToken() const {return isEnd() ? std::string() : tokens[curTokenIndex];}

         /*
            Consumes all tokens in a single call.

            Input: (none)
            Output: Vector containing all tokens (vector<string>)
         */
         inline std::vector<std::string> consumeAll() {

            curTokenIndex = tokens.size();
            return tokens;
         }

         /*
            Advances the current token.

            Input: (none)
            Output: (none)
         */
         inline void next() {curTokenIndex++;}

         /*
            Backtracks the current token.

            Input: (none)
            Output: (none)
         */
         inline void previous() {

            if (curTokenIndex > 0) {
               curTokenIndex--;
            }
         }

         /*
            Rewinds the tokenizer to the beginning of the string.

            Input: (none)
            Output: (none)
         */
         inline void rewind() {curTokenIndex = 0;}
   };
}


#endif
