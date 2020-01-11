#ifndef EXCEPTION_H
#define EXCEPTION_H


#include <stdexcept>


namespace trogdor {


   class Exception: public std::runtime_error {

      public:

         using runtime_error::runtime_error;
   };
}


#endif
