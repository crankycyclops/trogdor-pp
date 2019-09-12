#include "../include/iostream/nullin.h"


using namespace std;

namespace trogdor { namespace core {


   Trogin &NullIn::operator>> (string &val) {

      val = "";
   }

   std::unique_ptr<Trogin> NullIn::clone() {

      return std::make_unique<NullIn>();
   }
}}

