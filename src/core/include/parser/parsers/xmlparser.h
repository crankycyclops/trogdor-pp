#ifndef XMLPARSER_H
#define XMLPARSER_H


#include <memory>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <cstdlib>

#include <libxml/xmlreader.h>

#include "../../utility.h"
#include "../../vocabulary.h"
#include "../../entitymap.h"
#include "../../entities/player.h"
#include "../../lua/luastate.h"

#include "../parser.h"


using namespace std;

namespace trogdor {

   /*
      Parses an XML game definition.
   */
   class XMLParser: public Parser {

      private:

         // handles the actual XML parsing
         xmlTextReaderPtr reader;

         // When nextTag() encounters a closing tag, it's stored here.  Later,
         // if this has a value, checkClosingTag() will use it instead of
         // walking the XML tree.
         string lastClosedTag;

         /*
            Returns the name of the current XML tag.  Characters in the tag name
            are all converted to lowercase.

            Input:
               (none)

            Output:
               name of the tag
         */
         inline string getTagName() {

            return strToLower((const char *)xmlTextReaderConstName(reader));
         }

         /*
            Returns the current tag's depth in the XML tree.

            Input:
               (none)

            Output:
               current tag's depth -- i.e. how deeply nested it is (int)
         */
         inline int getDepth() {return xmlTextReaderDepth(reader);}

         /*
            Advances position in XML to the next opening tag.  If the next tag
            is an unbalanced closing tag, or if there's a parse error, an
            exception will be thrown.

            Input:
               (none)

            Output:
               True if another tag was parsed and false if there was nothing
               left to parse.

            Exceptions:
               Exception is thrown if the next tag isn't opening or there's a
               parse error.
         */
         bool nextTag();

         string getAttribute(const char *name);

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

         /*
            Parses a list of aliases, alternate identifiers for Things.

            Input:
               Name of Thing (string)
               Whether we're parsing aliases for Entity of Entity class (enum ParseMode)
               Parse depth in XML file

            Output:
               (none)
         */
         void parseThingAliases(string entityName, enum ParseMode mode, int depth);

         /*
            Parses auto-attack settings for a Creature.

            Input:
               Name of Creature (string)
               Are we parsing an entity or entity class? enum ParseMode mode)
               depth in the XML tree (int)

            Output:
               (none)
         */
         void parseCreatureAutoAttack(string creatureName, enum ParseMode mode,
         int depth);

         /*
            Parses a Creature's wandering settings.

            Input:
               Name of Creature (string)
               Whether we're parsing an Entity of Entity class (enum ParseMode)

            Output:
               (none)
         */
         void parseCreatureWandering(string creatureName, enum ParseMode mode);

         /*
            Parses a Being's respawn settings.

            Input:
               Name of being whose respawn settings are being configured
               Whether we're parsing for an Entity, Entity class, or default player
               Depth in the XML tree

            Output:
               (none)
         */
         void parseBeingRespawn(string beingName, enum ParseMode mode, int depth);

         /*
            Parses a Being's inventory settings.

            Input:
               Name of being whose respawn settings are being configured
               Whether we're parsing for an Entity, Entity class, or default player
               Whether or not to allow a set of objects initialized in game.xml

            Output:
               (none)
         */
         void parseBeingInventory(string beingName, enum ParseMode mode, bool allowObjects);

         /*
            Parses a Being's Attributes.

            Input:
               Name of Being whose attributes are being set
               Whether we're parsing an Entity or an Entity class (enum ParseMode)

            Output:
               (none)
         */
         void parseBeingAttributes(string beingName, enum ParseMode mode);

         /*
            Takes tags such as <north>, <south>, etc. in a room definition and
            forges that connection.

            Input:
               direction (string)
               Name of room where the connection should be made (string)
               Name of room that we want to connect to (string)
               Whether we're parsing an Entity or an Entity class (enum ParseMode)

            Output:
               (none)
         */
         void parseRoomConnection(string direction, string roomName, string connectTo, enum ParseMode mode);

         /*
            Parses a Room's version of the <contains> section.

            Input:
               Name of room into which we're inserting the object (string)
               Whether we're parsing an Entity or an Entity class (enum ParseMode)

            Output:
               (none)
         */
         void parseRoomContains(string roomName, enum ParseMode mode);

         /*
            Parse the contents of a Messages object from XML.

            Input:
               Name of Entity we're parsing messages for (string)
               Whether we're parsing an Entity or an Entity class (enum ParseMode)
               XML Depth--how deeply nested <message> tags will be (int)

            Output:
               (none)
         */
         void parseMessages(string entityName, enum ParseMode mode, int depth);

         /*
            Parses tags that should be applied to an Entity.

            Input:
               Name of Entity we're parsing messages for (string)
               Whether we're parsing an Entity or an Entity class (enum ParseMode)
               XML Depth--how deeply nested <message> tags will be (int)

            Output:
               (none)
         */
         void parseEntityTags(string entityName, enum ParseMode mode, int depth);

         /*
            Parses an <events> section for Entities, Entity classes, and Game.
            Throws an exception if there's an error.

            Input:
               Entity or Entity class name (string -- ignored if mode = PARSE_GAME)
               Whether we're parsing an Entity, an Entity class, or Game (enum ParseMode)
                  . Valid values are PARSE_ENTITY, PARSE_CLASS, and PARSE_GAME
               Depth in XML tree (int)

            Output:
               (none)
         */
         void parseEvents(string entityName, enum ParseMode mode, int depth);
         void parseScript(string entityName, enum ParseMode mode);
         void parseEvent(string entityName, enum ParseMode mode);

         /*
            This group of functions parses the <classes> section of the XML
            file.

            Input:
               (none)

            Output:
               (none)
         */
         void parseClasses();
         void parseClassesRooms();
         void parseClassesRoom();
         void parseClassesObjects();
         void parseClassesObject();
         void parseClassesCreatures();
         void parseClassesCreature();

         /*
            This group of functions parses the <introduction> section of the XML
            file, should it exist.

            Input:
               (none)

            Output:
               (none)
         */
         void parseIntroduction();

         /*
            This group of functions parses the <manifest> section of the XML
            file.

            Input:
               (none)

            Output:
               (none)
         */
         void parseManifest();
         void parseManifestRooms();
         bool parseManifestRoom(string className = "room");  // returns true if room's name is "start"
         void parseManifestCreatures();
         void parseManifestCreature(string className = "creature");
         void parseManifestObjects();
         void parseManifestObject(string className = "object");

         /*
            Parses meta data for entities or for the game.

            Input:
               (none)

            Output:
               (none)
         */
         void parseGameMeta();
         void parseEntityMeta(string entityName, enum ParseMode mode, int depth);

         /*
            Parses the vocabulary section of game.xml.

            Input:
               (none)

            Output:
               (none)
         */
         void parseVocabulary();

         /*
            Parses new custom directions (like north, south, east, etc.)

            Input:
               (none)

            Output:
               (none)
         */
         void parseDirections();

         /*
            Parses verb synonyms.

            Input:
               (none)

            Output:
               (none)
         */
         void parseSynonyms();

         /*
            Parse the Player section.  Throws an exception with an error message
            if there's a problem.

            Input:
               (none)

            Output:
               (none)
         */
         void parsePlayer();

         /*
            Parses settings for the default player.  As always, throws an
            exception with an error message if there's a problem.

            Input:
               (none)

            Output:
               (none)
         */
         void parseDefaultPlayer();

         /*
            Parses objects in the objects section of game.xml.  Throws an
            exception with an error message if there's a problem.

            Input:
               The object's class (string)

            Output:
               (none)
         */
         void parseObjects();
         void parseObject(string className = "object");
         void parseObjectProperties(string name, enum ParseMode mode, int depth);

         /*
            Parses the creatures section of game.xml.

            Input:
               Tag that closes the creature definition

            Output:
               (none)
         */
         void parseCreatures();
         void parseCreature(string className = "creature");
         void parseCreatureProperties(string name, enum ParseMode mode, int depth);

         /*
            Parses room definitions in game.xml.

            Input:
               Tag that closes the room definition

            Output:
               (none)
         */
         void parseRooms();
         void parseRoom(string className = "room");
         void parseRoomProperties(string name, enum ParseMode mode, int depth);

         /*
            Parses the <game> section of the XML file.  Throws an exception if
            there's a parse error.

            Input:
               (none)

            Output:
               Being's woundRate (double)
         */
         void parseGame();

      public:

         /*
            Constructor
         */
         inline XMLParser(std::unique_ptr<Instantiator> i, const Vocabulary &v):
         Parser(std::move(i), v) {

            reader = nullptr;
         }

         /*
            Destructor
         */
         ~XMLParser();

         /*
            Parses an XML game definition and passes the dataa off to an
            instance of Instantiator, which creates the corresponding data
            structures.

            Input:
               Filename where the game definition is saved (string)

            Output:
               (none)
         */
         virtual void parse(string filename);
   };
}


#endif

