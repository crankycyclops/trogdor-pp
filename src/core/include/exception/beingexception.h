#ifndef BEING_EXCEPTION_H
#define BEING_EXCEPTION_H


#include "entityexception.h"


using namespace std;

namespace trogdor { namespace entity {


   /*
      Throw this when there's an issue with a Being-type Entity specifically.
   */
   class BeingException: public EntityException {

      public:

         // When a BeingException occurs, an error code should be set to
         // indicate the nature of the failure
         enum errorCodeType {
            NONE,             // default value if we're not using this
            TAKE_TOO_HEAVY,   // couldn't take an item because it was too heavy
            TAKE_UNTAKEABLE,  // couldn't take an item because it was untakeable
            DROP_UNDROPPABLE  // couldn't drop an item because it was undroppable
         };

      protected:

         // Error code associated with the exception
         errorCodeType errorCode;

      public:

         // Contructors
         explicit BeingException(const string &what_arg);
         explicit BeingException(const char *what_arg);
         explicit BeingException(const string &what_arg, enum errorCodeType code);
         explicit BeingException(const char *what_arg, enum errorCodeType code);

         /*
            Returns the error code associated with the exception.

            Input:
               (none)

            Output:
               Error code (enum errorCodeType)
         */
         inline const enum errorCodeType getErrorCode() const noexcept {return errorCode;}
   };
}}


#endif

