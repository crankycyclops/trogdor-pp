#ifndef STREAMERR_H
#define STREAMERR_H


#include <memory>
#include <trogdor/iostream/trogerr.h>



/*
   Error output "stream" that wraps around the specified ostream object.
*/
class StreamErr: public trogdor::Trogerr {

   private:

      std::ostream *stream; // output stream where flush() sends data

   public:

      explicit inline StreamErr(std::ostream *s) {stream = s;}

      /*
         If I don't have this, I get undefined vtable errors when linking to
         libtrogdor.
      */
      virtual ~StreamErr();

      /*
         See include/iostream/trogout.h for details.
      */
      virtual void flush();

      /*
         See include/iostream/trogerr.h for details.
      */
      virtual std::unique_ptr<trogdor::Trogerr> copy();
};


#endif
