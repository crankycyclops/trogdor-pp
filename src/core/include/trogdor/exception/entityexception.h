#ifndef ENTITY_EXCEPTION_H
#define ENTITY_EXCEPTION_H


#include <trogdor/exception/exception.h>


namespace trogdor { namespace entity {


   /*
      Throw this when an Entity-related exception arises.
   */
   class EntityException: public Exception {

      public:

         using Exception::Exception;
   };
}}


#endif
