#ifndef DUPLICATE_ENTITY_EXCEPTION_H
#define DUPLICATE_ENTITY_EXCEPTION_H


#include <trogdor/exception/entityexception.h>


namespace trogdor::entity {


   /*
      Throw this when attempting to insert a duplicate entity.
   */
   class DuplicateEntity: public EntityException {

      public:

         using EntityException::EntityException;
   };
}


#endif
