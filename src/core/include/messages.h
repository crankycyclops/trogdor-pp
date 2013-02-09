#include <string>
#include <boost/unordered_map.hpp>

using namespace std;
using namespace boost;

namespace core {

   class Messages {

      private:

         // Hash table mapping message names to messages
         unordered_map<string, string> messageTable;

      public:

         /*
            Gets a message by name.  If it doesn't exist, an exception will be
            thrown.

            Input: string (name)
            Output: string (message)
         */
         string get(const string name) const;

         /*
            Sets a message.  If the message already exists, it will be
            overwritten.

            Input: string (message name), string (message)
            Output: (none)
         */
         void set(string name, string message);

         /*
            Displays a message.  If it doesn't exist, an exception will be
            thrown.

            Input: string (message name), output stream (ostream)
            Output: (none)
         */
         void display(string name, ostream &out) const;
   };
}

