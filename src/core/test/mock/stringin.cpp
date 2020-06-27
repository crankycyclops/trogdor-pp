#include "stringin.h"


trogdor::Trogin &StringIn::operator>> (std::string &val) {

   val = input;
   return *this;
}

std::unique_ptr<trogdor::Trogin> StringIn::clone() {

   return std::make_unique<StringIn>(input);
}
