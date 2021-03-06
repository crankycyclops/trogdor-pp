#include <trogdor/exception/parseexception.h>

namespace trogdor {


   ParseException::ParseException(const std::string &what_arg): Exception(what_arg) {

      filename = "";
      lineno = 0;
   }

   /***************************************************************************/

   ParseException::ParseException(const char *what_arg): Exception(what_arg) {

      filename = "";
      lineno = 0;
   }

   /***************************************************************************/

   ParseException::ParseException(const std::string &what_arg, std::string f, int l):
   Exception(what_arg) {

      filename = f;
      lineno = l;
   }

   /***************************************************************************/

   ParseException::ParseException(const char *what_arg, std::string f, int l):
   Exception(what_arg) {

      filename = f;
      lineno = l;
   }

   /***************************************************************************/

   const char *ParseException::what() const noexcept {

      // Since I'm returning a C string, I need to make sure the combined error
      // message remains in memory after this method returns
      static std::string combinedMsg;

      combinedMsg = "";

      if (0 != filename.compare("")) {
         combinedMsg += filename + ": ";
      }

      combinedMsg += Exception::what();

      if (lineno) {
         combinedMsg += " (line " + std::to_string(lineno) + ")";
      }

      return combinedMsg.c_str();
   }
}
