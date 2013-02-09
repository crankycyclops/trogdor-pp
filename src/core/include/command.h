#include <iostream>
#include <string>

#include "tokenizer.h"

using namespace std;

namespace core {

   class Command {

      private:

         // Note that a command is presumed invalid until successfully parsed
         bool invalid;

         string verb;
         string directObject;
         string indirectObject;
         string preposition;

         void parse(string commandStr);

         int parseDirectObject(Tokenizer *tokenizer);

         int parseIndirectObject(Tokenizer *tokenizer);

         bool isPreposition(const string word);

         bool isFillerWord(const string word);

      public:

         /*
            Constructor for the Command class.
         */
         Command();

         /*
            Returns true if the command is invalid.

            Input: (none)
            Output: (none)
         */
         inline bool isInvalid() const {return invalid;}

         /*
            Getters for the syntactic components of a command.
         */
         inline string getVerb() const {return verb;}
         inline string getDirectObject() const {return directObject;}
         inline string getindirectObject() const {return indirectObject;}
         inline string getPreposition() const {return preposition;}

         /*
            Reads a command from the user.

            Input: input stream (istream) and output stream (ostream)
            Output: (none)
         */
         void read(istream &in, ostream &out);

         /*
            Executes the command.

            Input: (none)
            Output: (none)
         */
         void execute();
   };
}

