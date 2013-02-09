using namespace std;

#include "include/tokenizer.h"

namespace core {


   Tokenizer::Tokenizer(const char *str) {

      end = false;
      wholeStr = str;
      curToken = strtok((char *)str, " \t");
   }


   void Tokenizer::next() {

      const char *token = strtok(NULL, " \t");

      if (NULL == token) {
         end = true;
      }

      else {
         curToken = token;
      }
   }


   void Tokenizer::rewind() {

      end = false;
      curToken = strtok((char *)wholeStr.c_str(), " \t");
   }
}

