#include "../include/iostream/nullin.h"


using namespace std;

namespace core {


   Trogin &NullIn::operator>> (string &val) {

      val = "";
   }

   Trogin *NullIn::clone() {

      return new NullIn();
   }
}

