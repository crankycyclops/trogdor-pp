#ifndef PHP_STREAMERR_H
#define PHP_STREAMERR_H


extern "C" {
	#include "php.h"
}

#include <memory>
#include <trogdor/iostream/trogerr.h>

#include "exception.h"


// PHP output stream for error messages.
class PHPStreamErr: public trogdor::Trogerr {

	public:

		// See core/include/iostream/trogout.h for details.
		virtual void flush();

		// See core/include/iostream/trogerr.h for details.
		virtual std::unique_ptr<trogdor::Trogerr> copy();
};


#endif /* PHP_STREAMERR_H */
