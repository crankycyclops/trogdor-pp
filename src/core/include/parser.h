#ifndef PARSER_H
#define PARSER_H


#include <iostream>
#include <string>
#include <cstdlib>

#include <libxml/xmlreader.h>

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

