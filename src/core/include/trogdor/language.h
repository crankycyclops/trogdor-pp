#ifndef LANGUAGE_H
#define LANGUAGE_H


#include <string>


using namespace std;

namespace trogdor {


   class Language {

      public:

         /*
            Takes as input a singular noun and makes it plural according to
            language-specific rules.

            Input:
               Singular noun (string)

            Output:
               Plural noun (string)
         */
         virtual string pluralizeNoun(string noun) const = 0;
   };
}


#endif
