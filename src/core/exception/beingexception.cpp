#include <trogdor/exception/beingexception.h>

namespace trogdor { namespace entity {


   BeingException::BeingException(const std::string &what_arg): EntityException(what_arg) {

      errorCode = NONE;
   }

   /***************************************************************************/

   BeingException::BeingException(const char *what_arg): EntityException(what_arg) {

      errorCode = NONE;
   }

   /***************************************************************************/

   BeingException::BeingException(const std::string &what_arg, ErrorCodeType code):
   EntityException(what_arg) {

      errorCode = code;
   }

   /***************************************************************************/

   BeingException::BeingException(const char *what_arg, ErrorCodeType code):
   EntityException(what_arg) {

      errorCode = code;
   }
}}
