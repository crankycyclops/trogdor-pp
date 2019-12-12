#include "../include/iostream/nullin.h"


namespace trogdor {


   Trogin &NullIn::operator>> (std::string &val) {

      val = "";
      return *this;
   }

   std::unique_ptr<Trogin> NullIn::clone() {

      return std::make_unique<NullIn>();
   }
}
