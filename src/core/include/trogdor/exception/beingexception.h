#ifndef BEING_EXCEPTION_H
#define BEING_EXCEPTION_H


#include <trogdor/exception/entityexception.h>


namespace trogdor::entity {


   /*
      Throw this when there's an issue with a Being-type Entity specifically.
   */
   class BeingException: public EntityException {

      public:

         // When a BeingException occurs, an error code should be set to
         // indicate the nature of the failure
         enum ErrorCodeType {
            NONE,             // default value if we're not using this
            TAKE_TOO_HEAVY,   // couldn't take an item because it was too heavy
            TAKE_UNTAKEABLE,  // couldn't take an item because it was untakeable
            DROP_UNDROPPABLE  // couldn't drop an item because it was undroppable
         };

      protected:

         // Error code associated with the exception
         ErrorCodeType errorCode;

      public:

         // Contructors
         explicit BeingException(const std::string &what_arg);
         explicit BeingException(const char *what_arg);
         explicit BeingException(const std::string &what_arg, ErrorCodeType code);
         explicit BeingException(const char *what_arg, ErrorCodeType code);

         /*
            Returns the error code associated with the exception.

            Input:
               (none)

            Output:
               Error code (ErrorCodeType)
         */
         inline const enum ErrorCodeType getErrorCode() const noexcept {return errorCode;}
   };
}


#endif
