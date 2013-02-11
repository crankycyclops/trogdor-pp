#ifndef PARSER_H
#define PARSER_H


#include <iostream>
#include <string>
#include <cstdlib>

#include <libxml/xmlreader.h>

using namespace std;

namespace core {


   /*
      Parses the game.xml file and populates the containing Game object with
      the corresponding data structures.
   */
   class Parser {

      private:

         xmlTextReaderPtr reader;   // handles the actual XML parsing

      public:

         /*
            Constructor for the Parser class.  Takes as input the filename of
            the game XML file and opens the file for reading.
         */
         Parser(string gameFile = "game.xml");

         /*
            Destructor for the Parser class.
         */
         ~Parser();
   };
}


#endif

