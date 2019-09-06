#include "include/streamin.h"


using namespace std;

namespace trogdor { namespace standalone {


	trogdor::core::Trogin &StreamIn::operator>> (string &val) {

	   getline(*stream, val);
	}

	trogdor::core::Trogin *StreamIn::clone() {

	   return new StreamIn(stream);
	}
}}

