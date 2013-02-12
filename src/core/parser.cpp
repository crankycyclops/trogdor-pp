#include "include/parser.h"

using namespace std;

namespace core {


   Parser::Parser(string gameFile) {

      reader = xmlReaderForFile(gameFile.c_str(), NULL, XML_PARSE_NOBLANKS);
      if (NULL == reader) {
         throw "failed to open " + gameFile + "!\n";
      }
   }

   /***************************************************************************/

   Parser::~Parser() {

      // TODO: do I free entity objects inside?
      xmlFreeTextReader(reader);
   }

   /***************************************************************************/

   void Parser::parse() {

      // TODO
   }
}

