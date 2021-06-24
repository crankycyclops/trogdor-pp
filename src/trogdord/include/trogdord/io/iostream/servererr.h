#ifndef SERVERERR_H
#define SERVERERR_H


#include <memory>
#include <trogdor/iostream/trogerr.h>



/*
   Error output "stream" that wraps around the specified ostream object.
*/
class ServerErr: public trogdor::Trogerr {

   private:

      std::ostream *stream; // output stream where flush() sends data

   public:

      explicit inline ServerErr(std::ostream *s) {stream = s;}

      /*
         Tests whether or not the passed in stream is equal to the stream object
         used by the instance of ServerErr. This is intended for testing.
      */
      inline bool streamEquals(std::ostream *s) const {return s == stream;}

      /*
         If I don't have this, I get undefined vtable errors when linking to
         libtrogdor.
      */
      virtual ~ServerErr();

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
