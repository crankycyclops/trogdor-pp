#ifndef UNDEFINED_EXCEPTION_H
#define UNDEFINED_EXCEPTION_H


#include <trogdor/exception/exception.h>


namespace trogdor {


   /*
      Throw this when the code attempts to do something that's unsupported or
      undefined.
   */
   class UndefinedException: public Exception {

      public:

         using Exception::Exception;
   };
}


#endif
