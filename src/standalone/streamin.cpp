#include "include/streamin.h"


using namespace std;


core::Trogin &StreamIn::operator>> (string &val) {

   getline(*stream, val);
}

core::Trogin *StreamIn::clone() {

   return new StreamIn(stream);
}

