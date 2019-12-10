#include "include/tokenizer.h"

namespace trogdor {


   Tokenizer::Tokenizer(string s) {

      std::regex re("\\s+");
      std::sregex_token_iterator curToken(s.begin(), s.end(), re, -1);
      std::sregex_token_iterator endOfTokens;

      for (; curToken != endOfTokens; ++curToken) {
         tokens.push_back(curToken->str());
      }

      rewind();
   }
}

