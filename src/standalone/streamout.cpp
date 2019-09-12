#include <memory>
#include "include/streamout.h"


using namespace std;

namespace trogdor { namespace standalone {


	void StreamOut::flush() {

	   *stream << getBufferStr();
	   stream->flush();
	   clear();
	}

	std::unique_ptr<trogdor::core::Trogout> StreamOut::clone() {

	   return std::make_unique<StreamOut>(stream);
	}
}}

