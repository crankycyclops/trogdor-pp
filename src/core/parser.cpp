#include "include/parser.h"

using namespace std;

namespace core {


   Parser::Parser(string gameFile) {

      filename = gameFile;

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

   /***************************************************************************/

   bool Parser::parseBool() {

      stringstream s;

      s << filename << ": Expecting boolean 1 for true or 0 for false (line "
         << xmlTextReaderGetParserLineNumber(reader) << ")";

      try {
         return boost::lexical_cast<bool>(getNodeValue());
      }

      catch (boost::bad_lexical_cast) {
         throw s.str();
      }
   }

   /***************************************************************************/

   int Parser::parseInt() {

      stringstream s;

      s << filename << ": Expecting integer value (line "
         << xmlTextReaderGetParserLineNumber(reader) << ")";

      try {
         return boost::lexical_cast<int>(getNodeValue());
      }

      catch (boost::bad_lexical_cast) {
         throw s.str();
      }
   }

   /***************************************************************************/

   double Parser::parseDouble() {

      stringstream s;

      s << filename << ": Expecting floating point number (line "
         << xmlTextReaderGetParserLineNumber(reader) << ")";

      try {
         return boost::lexical_cast<double>(getNodeValue());
      }

      catch (boost::bad_lexical_cast) {
         throw s.str();
      }
   }

   /***************************************************************************/

   string Parser::parseString() {

      stringstream s;
      string value = getNodeValue();

      s << filename << ": Tag requires a value (line "
         << xmlTextReaderGetParserLineNumber(reader) << ")";

      if (value.length() > 0) {
         return value;
      }

      throw s.str();
   }

   /***************************************************************************/

   const char *Parser::getNodeValue() {

      stringstream s;

      // get the corresponding #text node for the value
      if (xmlTextReaderRead(reader) < 0) {
         s << "Unknown error reading " << filename << " (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      else if (0 != strcmp("#text", (const char *)xmlTextReaderConstName(reader))) {
         s << "tag must be given a value (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      return (const char *)xmlTextReaderValue(reader);
   }

   /***************************************************************************/

   void Parser::checkClosingTag(string tag) {

      stringstream s;

      if (xmlTextReaderRead(reader) < 0) {
         s << "Unknown error reading " << filename << " (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      else if (XML_ELEMENT_DECL != xmlTextReaderNodeType(reader) ||
      0 != strcmp(tag.c_str(), (const char *)xmlTextReaderConstName(reader))) {
         s << "missing closing </" << tag << "> (line " <<
            xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }
   }
}

