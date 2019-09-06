#ifndef STREAMOUT_H
#define STREAMOUT_H


#include "../../core/include/iostream/trogout.h"


using namespace std;

namespace trogdor { namespace standalone {


	/*
	 Output "stream" that wraps around the specified ostream object. This
	 particular object happens to ignore channels.
	*/
	class StreamOut: public trogdor::core::Trogout {

	   private:

		  ostream *stream; // output stream where flush() sends data

	   public:

		  inline StreamOut(ostream *s) {stream = s;}

		  /*
		   See include/iostream/trogout.h for details.
		  */
		  virtual void flush();

		  /*
		   See include/iostream/trogout.h for details.
		  */
		  virtual trogdor::core::Trogout *clone();
	};
}}


#endif

