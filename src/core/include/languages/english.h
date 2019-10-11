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

      private:

         // Maps one-off exceptions to the pluralization rules (ex: die => dice)
         unordered_map<string, string> pluralNounExceptions;

         // Plurals that can be regularly changed according to simple regex
         // substitution rules (ex: the suffix "ouse$" => "ice")
         unordered_map<string, string> regexReplacements;

      public:

         /*
            Constructor
         */
         English();

         /*
            Takes as input a singular noun and makes it plural according to the
            rules of English.

            This method DOES NOT handle pronouns, and the result of passing in
            a pronoun is undefined.

            Input:
               Singular noun (string)

            Output:
               Plural noun (string)
         */
         virtual string pluralizeNoun(string noun) const;
   };
}


#endif

