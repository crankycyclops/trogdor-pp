#include "include/tokenizer.h"

using namespace std;

namespace trogdor {


   Tokenizer::Tokenizer(string s) {

      regex re("\\s+");
      sregex_token_iterator curToken(s.begin(), s.end(), re, -1);
      sregex_token_iterator endOfTokens;

      for (; curToken != endOfTokens; ++curToken) {
         tokens.push_back(curToken->str());
      }

      rewind();
   }
}

