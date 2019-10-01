#include <memory>

#include "../../include/utility.h"
#include "../../include/vocabulary.h"

#include "../../include/parser/parsers/inform7parser.h"
#include "../../include/exception/parseexception.h"

namespace trogdor {


   Inform7Parser::Inform7Parser(std::unique_ptr<Instantiator> i,
   const Vocabulary &v): Parser(std::move(i), v) {

      // TODO
   }

   /**************************************************************************/

   Inform7Parser::~Inform7Parser() {

      // TODO
   }

   /**************************************************************************/

   void Inform7Parser::parse(string filename) {

      throw ParseException("Inform 7 support not yet implemented.");
   }
}

