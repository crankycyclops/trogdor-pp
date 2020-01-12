#ifndef STREAMERR_H
#define STREAMERR_H


#include <memory>
#include <trogdor/iostream/trogout.h>



/*
   PHP Output stream for error messages.
*/
class PHPStreamErr: public trogdor::Trogout {

   public:

      /*
         See core/include/iostream/trogout.h for details.
      */
      virtual void flush();

      /*
         See core/include/iostream/trogout.h for details.
      */
      virtual std::unique_ptr<trogdor::Trogout> clone();
};


#endif
