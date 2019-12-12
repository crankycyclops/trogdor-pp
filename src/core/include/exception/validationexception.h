#ifndef VALIDATION_EXCEPTION_H
#define VALIDATION_EXCEPTION_H


#include "exception.h"


namespace trogdor {


   /*
      Throw this whenever calling functions/methods with invalid data/arguments.
   */
   class ValidationException: public Exception {

      public:

         using Exception::Exception;
   };
}


#endif
