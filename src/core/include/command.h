#include <string>

using namespace std;

namespace core {

   class Command {

      private:

         bool error;

         string verb;
         string directObject;
         string indirectObject;
         string preposition;

      public:

         Command();

         inline bool isError() const {return error;}
   };
}

