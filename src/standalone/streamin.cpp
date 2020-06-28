#include "include/streamin.h"


trogdor::Trogin &StreamIn::operator>> (std::string &val) {

   getline(*stream, val);
   return *this;
}

std::unique_ptr<trogdor::Trogin> StreamIn::clone() {

   return std::make_unique<StreamIn>(stream);
}
