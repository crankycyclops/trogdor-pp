using namespace std;

#include <iterator>
#include "include/messages.h"

namespace core {


   string Messages::get(const string name) const {

      if (messageTable.find(name) == messageTable.end()) {
         return "";
      }

      return messageTable.find(name)->second;
   }


   void Messages::set(string name, string message) {

      messageTable[name] = message;
      return;
   }


   void Messages::display(string name) const {

      string message = get(name);

      cout << message;
      if (message.length() > 0) {
         cout << '\n';
      }

      return;
   }
}
