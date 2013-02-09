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
            Executes the command.

            Input: (none)
            Output: (none)
         */
         void execute();
   };
}

