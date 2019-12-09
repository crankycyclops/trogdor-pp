#include "../include/iostream/nullin.h"


using namespace std;

namespace trogdor {


   Trogin &NullIn::operator>> (string &val) {

      val = "";
      return *this;
   }

   std::unique_ptr<Trogin> NullIn::clone() {

      return std::make_unique<NullIn>();
   }
}

