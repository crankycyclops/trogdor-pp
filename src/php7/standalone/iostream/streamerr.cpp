#include "streamerr.h"


void PHPStreamErr::flush() {

	#ifdef ZTS

		if (trogdor::Trogerr::ERROR == getErrorLevel()) {
			zend_throw_exception(EXCEPTION_GLOBALS(baseException), getBufferStr().c_str(), 0);
		}

		else if (trogdor::Trogerr::WARNING == getErrorLevel()) {
			php_error_docref(NULL, E_WARNING, getBufferStr().c_str());
		}

		else if (trogdor::Trogerr::INFO == getErrorLevel()) {
			php_error_docref(NULL, E_NOTICE, getBufferStr().c_str());
		}

	#else

		// TODO: for non-ZTS builds, offer an option for appending errors to
		// a log file (configurable via php.ini.)
		std::cerr << errorLevelToStr(getErrorLevel()) << ": " + getBufferStr() << std::endl;

	#endif
}

std::unique_ptr<trogdor::Trogerr> PHPStreamErr::copy() {

	return std::make_unique<PHPStreamErr>();
}
