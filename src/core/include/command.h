#include <iostream>
#include <string>

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
            Executes the command.

            Input: (none)
            Output: (none)
         */
         void execute();

         /*
            Reads a command and returns a Command object to represent it.

            Input: input stream (istream) and output stream (ostream)
            Output: Command object
         */
         static Command *read(istream &in, ostream &out);
   };
}

