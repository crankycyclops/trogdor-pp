#ifndef STREAMOUT_H
#define STREAMOUT_H


#include <memory>
#include "../../core/include/iostream/trogout.h"



/*
   Output "stream" that wraps around the specified ostream object. This
   particular object happens to ignore channels.
*/
class StreamOut: public trogdor::Trogout {

   private:

      std::ostream *stream; // output stream where flush() sends data

   public:

      inline StreamOut(std::ostream *s) {stream = s;}

      /*
         See include/iostream/trogout.h for details.
      */
      virtual void flush();

      /*
         See include/iostream/trogout.h for details.
      */
      virtual std::unique_ptr<trogdor::Trogout> clone();
};


#endif
