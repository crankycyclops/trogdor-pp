#include "include/tokenizer.h"

using namespace std;

namespace core {


   void Tokenizer::init(const char *str) {

      const char *token;

      end = false;
      wholeStr = str;

      token = strtok((char *)str, " \t");

      if (NULL == token) {
         end = true;
      }

      else {
         curToken = token;
      }
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

