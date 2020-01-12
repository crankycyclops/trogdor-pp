extern "C" {
	#include "php.h"
}

#include <memory>
#include "streamerr.h"


void PHPStreamErr::flush() {

	php_error_docref(NULL, E_ERROR, getBufferStr().c_str());
	clear();
}

std::unique_ptr<trogdor::Trogout> PHPStreamErr::clone() {

	return std::make_unique<PHPStreamErr>();
}
