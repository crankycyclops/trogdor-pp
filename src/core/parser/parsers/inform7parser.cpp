#include <memory>

#include "../../include/utility.h"

#include "../../include/parser/parsers/inform7parser.h"
#include "../../include/exception/parseexception.h"

namespace trogdor {


   Inform7Parser::~Inform7Parser() {

      // TODO
   }

   /**************************************************************************/

   void Inform7Parser::parse(string filename) {

      throw ParseException("Inform 7 support not yet implemented.");
   }
}

