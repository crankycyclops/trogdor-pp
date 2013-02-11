#include "include/parser.h"

using namespace std;

namespace core {


   Parser::Parser(string gameFile) {

      reader = xmlReaderForFile(gameFile.c_str(), NULL, XML_PARSE_NOBLANKS);
      if (NULL == reader) {
         throw;
      }
   }


   Parser::~Parser() {

      xmlFreeTextReader(reader);
   }
}

