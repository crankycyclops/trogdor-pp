#include <iostream>
#include <iterator>

#include <trogdor/messages.h>

namespace trogdor {


   std::string Messages::get(const std::string &name) const {

      if (messageTable.find(name) == messageTable.end()) {
         return "";
      }

      return messageTable.find(name)->second;
   }


   void Messages::set(std::string name, std::string message) {

      messageTable[name] = message;
      return;
   }


   void Messages::display(std::string name, std::ostream &out) const {

      std::string message = get(name);

      out << message;
      if (message.length() > 0) {
         out << '\n';
      }

      return;
   }
}

