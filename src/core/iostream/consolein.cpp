#include "../include/iostream/consolein.h"


using namespace std;

namespace core {


   Trogin &ConsoleIn::operator>> (string &val) {

      getline(cin, val);
   }

   Trogin *ConsoleIn::clone() {

      return new ConsoleIn();
   }
}

