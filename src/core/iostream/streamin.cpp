#include "../include/iostream/streamin.h"


using namespace std;

namespace core {


   Trogin &StreamIn::operator>> (string &val) {

      getline(*stream, val);
   }

   Trogin *StreamIn::clone() {

      return new StreamIn(stream);
   }
}

