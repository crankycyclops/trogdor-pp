#ifndef FILE_EXCEPTION_H
#define FILE_EXCEPTION_H


#include <trogdor/exception/exception.h>


namespace trogdor {


   /*
      Throw this when we have trouble with an operation involving a file.
   */
   class FileException: public Exception {

      public:

         using Exception::Exception;
   };
}


#endif
