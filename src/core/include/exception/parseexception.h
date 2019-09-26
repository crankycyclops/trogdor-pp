#ifndef PARSE_EXCEPTION_H
#define PARSE_EXCEPTION_H


#include <string>
#include "exception.h"


using namespace std;

namespace trogdor {


   /*
      Throw this when an exception arises during parsing.
   */
   class ParseException: public Exception {

      protected:

         // Filename and line number where the parsing error occurred (optional)
         string filename;
         int lineno;

      public:

         // Contructors
         explicit ParseException(const string &what_arg);
         explicit ParseException(const char *what_arg);
         explicit ParseException(const string &what_arg, string file, int line);
         explicit ParseException(const char *what_arg, string file, int line);

         /*
            The same as std::exception::what() except that it prepends the
            filename at the beginning (if it's set) and appends the line number
            (if it's set) at the end.

            Input:
               (none)

            Output:
               Error message + file and line number info if set (const char *)
         */
         virtual const char *what() const noexcept;

         /*
            Returns the filename that was being parsed when the exception
            occurred.

            Input:
               (none)

            Output:
               Filename (string)
         */
         inline const string getFilename() const noexcept {return filename;}

         /*
            Returns the line number in the file being parsed where the exception
            occurred.

            Input:
               (none)

            Output:
               Line number (int)
         */
         inline const int getLineNo() const noexcept {return lineno;}
   };
}


#endif

