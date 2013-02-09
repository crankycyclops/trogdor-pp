#ifndef TOKENIZER_H
#define TOKENIZER_H


#include <cstring>
#include <string>

using namespace std;

namespace core {

   class Tokenizer {

      private:

         bool   end;        // true if we've reached the end
         string wholeStr;   // string that we're tokenizing
         string curToken;   // the current token

         /*
            Called by the constructors to initialize the tokenizer.

            Input: C string (const char *)
            Output: (none)
         */
         void init(const char *str);

      public:

         /*
            Constructors for the tokenizer class
         */
         inline Tokenizer(const char *str) {init(str);}
         inline Tokenizer(string str) {init(str.c_str());}

         /*
            Retrieve the current token.

            Input: (none)
            Output: current token (string)
         */
         inline string getCurToken() const {return curToken;}

         /*
            Returns true if we've reached the end of the string.

            Input: (none)
            Output: bool
         */
         inline bool isEnd() const {return end;}

         /*
            Advances the current token.

            Input: (none)
            Output: (none)
         */
         void next();

         /*
            Rewinds the tokenizer to the beginning of the string.

            Input: (none)
            Output: (none)
         */
         void rewind();
   };
}


#endif

