#ifndef LUA_EXCEPTION_H
#define LUA_EXCEPTION_H


#include "exception.h"


namespace trogdor {


   /*
      Throw this when an exception occurs in Lua-related operations.
   */
   class LuaException: public Exception {

      public:

         using Exception::Exception;
   };
}


#endif
