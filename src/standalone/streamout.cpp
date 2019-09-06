#include "include/streamout.h"


using namespace std;

namespace trogdor { namespace standalone {


	void StreamOut::flush() {

	   *stream << getBufferStr();
	   stream->flush();
	   clear();
	}

	trogdor::core::Trogout *StreamOut::clone() {

	   return new StreamOut(stream);
	}
}}

