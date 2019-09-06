#include <iostream>
#include <iterator>

#include "include/messages.h"

using namespace std;

namespace trogdor { namespace core {


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


   void Messages::display(string name, ostream &out) const {

      string message = get(name);

      out << message;
      if (message.length() > 0) {
         out << '\n';
      }

      return;
   }
}}

