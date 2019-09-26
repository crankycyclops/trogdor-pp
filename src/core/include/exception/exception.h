#ifndef EXCEPTION_H
#define EXCEPTION_H


#include <stdexcept>


using namespace std;

namespace trogdor {


   class Exception: public runtime_error {

      public:

         using runtime_error::runtime_error;
   };
}


#endif

