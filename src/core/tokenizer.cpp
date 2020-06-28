#include <trogdor/utility.h>
#include <trogdor/tokenizer.h>

namespace trogdor {


   Tokenizer::Tokenizer(std::string s) {

      trim(s);

      if (s.length()) {

         std::regex re("\\s+");
         std::sregex_token_iterator curToken(s.begin(), s.end(), re, -1);
         std::sregex_token_iterator endOfTokens;

         for (; curToken != endOfTokens; ++curToken) {
            tokens.push_back(curToken->str());
         }
      }

      rewind();
   }
}
