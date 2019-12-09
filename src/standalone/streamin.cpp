#include <memory>
#include "include/streamin.h"


using namespace std;


trogdor::Trogin &StreamIn::operator>> (string &val) {

   getline(*stream, val);
   return *this;
}

std::unique_ptr<trogdor::Trogin> StreamIn::clone() {

   return std::make_unique<StreamIn>(stream);
}

