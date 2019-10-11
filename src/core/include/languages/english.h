#ifndef ENGLISH_H
#define ENGLISH_H


#include <string>
#include <unordered_map>

#include "../language.h"


using namespace std;

namespace trogdor {


   /*
      The pluralization rules, though influenced by other sources, are adapted
      in large part from the paper here:
      http://users.monash.edu/~damian/papers/HTML/Plurals.html
   */
   class English: public Language {

      public:

         /*
            Takes as input a singular noun and makes it plural according to the
            rules of English.

            Input:
               Singular noun (string)

            Output:
               Plural noun (string)
         */
         virtual string pluralizeNoun(string noun) const;
   };
}


#endif

