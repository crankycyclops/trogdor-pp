#ifndef PARSER_H
#define PARSER_H


#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <cstdlib>

#include <libxml/xmlreader.h>
#include <boost/lexical_cast.hpp>

#include "utility.h"
#include "entitymap.h"
#include "entities/player.h"


using namespace std;

namespace core {


   /*
      Parses the game.xml file and populates the containing Game object with
      the corresponding data structures.
   */
   class Parser {

      private:

         // handles the actual XML parsing
         xmlTextReaderPtr reader;

         // name of the file we're parsing
         string filename;

         // Player object representing default settings for all new players
         entity::Player *player;

         // hash tables for various types of entities
         // Note: the logical conclusion of having a hierarchical mapping of
         // object types is that no object of any type can share the same name!
         // This can be worked around via synonyms :)
         entity::EntityMap    entities;
         entity::PlaceMap     places;
         entity::ThingMap     things;
         entity::RoomMap      rooms;
         entity::BeingMap     beings;
         entity::ItemMap      items;
         entity::CreatureMap  creatures;
         entity::ObjectMap    objects;

         /*
            Returns the raw value of an XML tag.  Should not be called directly,
            but should instead be used by parseBool, parseInt, etc.  Throws an
            exception if there's a parsing error or if there's no value.

            Input:
               Name of closing tag to check for

            Output:
               (none)

            Exceptions:
               Throws exception if there's a parsing error or if there's no
               value.
         */
         const char *getNodeValue();

         /*
            Checks the current position in game.xml for a closing tag with the
            specified name and throws an exception if that closing tag is not
            found.  If the tag is found, we progress the current position in
            game.xml to just past the closing tag.

            Input:
               Name of closing tag to check for

            Output:
               (none)

            Exceptions:
               Throws exception if the closing tag is not found or there's an
               error
         */
         void checkClosingTag(string tag);

         /*
            Parses a boolean value from the last encountered XML tag and leaves
            the current position in the XML file just after the value.  Throws
            an exception if there's an error.

            Input:
               (none)

            Output:
               (none)

            Exceptions:
               Throws exception if there's a parsing error or if the value
               isn't 1 (for true) or 0 (for false.)
         */
         bool parseBool();

         /*
            Parses an integer value from the last encountered XML tag and leaves
            the current position in the XML file just after the value.  Throws
            an exception if there's an error.

            Input:
               (none)

            Output:
               (none)

            Exceptions:
               Throws exception if there's a parsing error or if the value
               isn't an integer.
         */
         int parseInt();

         /*
            Parses a double from the last encountered XML tag and leaves the
            current position in the XML file just after the value.  Throws an
            exception if there's an error.

            Input:
               (none)

            Output:
               (none)

            Exceptions:
               Throws exception if there's a parsing error or if the value
               isn't a valid number.
         */
         double parseDouble();

         /*
            Parses a string from the last encountered XML tag and leaves the
            current position in the XML file just after the value.  Throws an
            exception if there's an error.

            Input:
               (none)

            Output:
               (none)

            Exceptions:
               Throws exception if there's a parsing error or if there's no
               value for the current tag.
         */
         string parseString();

      public:

         /*
            Constructor for the Parser class.  Takes as input the filename of
            the game XML file and opens the file for reading.  If the specified
            file cannot be opened, an exception is thrown with an error message.
         */
         Parser(string gameFile);

         /*
            Destructor for the Parser class.
         */
         ~Parser();

         /*
            Parses XML game definition and constructs its corresponding game
            entities.  In the event of an error, an exception is thrown
            containing the error message.

            Input: (none)
            Output: (none)
         */
         void parse();
   };
}


#endif

