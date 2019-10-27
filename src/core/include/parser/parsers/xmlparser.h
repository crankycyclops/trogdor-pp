#ifndef XMLPARSER_H
#define XMLPARSER_H


#include <memory>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unordered_map>
#include <unordered_set>

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

         // Record corresponding to an Entity declared in <manifest>
         struct DeclaredEntity {
            string name;
            string className;
            entity::EntityType type;
         };

         // handles the actual XML parsing
         xmlTextReaderPtr reader;

         // When nextTag() encounters a closing tag, it's stored here.  Later,
         // if this has a value, checkClosingTag() will use it instead of
         // walking the XML tree.
         string lastClosedTag;

         // Keeps track of Entities that were declared in <manifest>
         unordered_map<string, DeclaredEntity> declaredEntities;

         // Keeps track of Entity classes that were defined in <classes>
         unordered_map<string, entity::EntityType> declaredEntityClasses;

         // Keeps track of custom directions and direction synonyms
         unordered_set<string> customDirections;

         /*
            Returns true if the specified Entity was declared and false if not.

            Input:
               (none)

            Output:
               Whether or not the Entity exists (bool)
         */
         inline bool entityDeclared(string name) {

            return declaredEntities.end() != declaredEntities.find(name) ? true : false;
         }

         /*
            Returns true if the specified Entity class was declared and is of
            the specified type and false if not.

            Input:
               (none)

            Output:
               Whether or not the Entity class of the specified type exists (bool)
         */
         inline bool entityClassDeclared(string name, entity::EntityType type) {

            if (declaredEntityClasses.end() == declaredEntityClasses.find(name)) {
               return false;
            }

            return type == declaredEntityClasses[name] ? true : false;
         }

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
               Type of thing we're parsing--one of "entity" or "class" (string)
               Parse depth in XML file

            Output:
               (none)
         */
         void parseThingAliases(string entityName, string targetType, int depth);

         /*
            Parses auto-attack settings for a Creature.

            Input:
               Name of Creature (string)
               Type of thing we're parsing--"entity" or "class" (string)
               depth in the XML tree (int)

            Output:
               (none)
         */
         void parseCreatureAutoAttack(string creatureName, string targetType,
         int depth);

         /*
            Parses a Creature's wandering settings.

            Input:
               Name of Creature (string)
               Type of thing we're parsing--"entity" or "class" (string)

            Output:
               (none)
         */
         void parseCreatureWandering(string creatureName, string targetType);

         /*
            Parses a Being's respawn settings.

            Input:
               Name of being whose respawn settings are being configured
               Type of thing we're parsing--"entity" or "class" (string)
               Depth in the XML tree

            Output:
               (none)
         */
         void parseBeingRespawn(string beingName, string targetType, int depth);

         /*
            Parses a Being's inventory settings.

            Input:
               Name of being whose respawn settings are being configured
               Type of thing we're parsing--"entity", "class", or "defaultPlayer"
               Whether or not to allow a set of objects initialized in game.xml

            Output:
               (none)
         */
         void parseBeingInventory(string beingName, string targetType,
         bool allowObjects);

         /*
            Parses a Being's Attributes.

            Input:
               Name of Being whose attributes are being set
               Type of thing we're parsing--"entity" or "class" (string)

            Output:
               (none)
         */
         void parseBeingAttributes(string beingName, string targetType);

         /*
            Takes tags such as <north>, <south>, etc. in a room definition and
            forges that connection.

            Input:
               direction (string)
               Name of room where the connection should be made (string)
               Name of room that we want to connect to (string)
               Type of thing we're parsing--"entity" or "class" (string)

            Output:
               (none)
         */
         void parseRoomConnection(string direction, string roomName,
         string connectTo, string targetType);

         /*
            Parses a Room's version of the <contains> section.

            Input:
               Name of room into which we're inserting the object (string)
               Type of thing we're parsing--"entity" or "class" (string)

            Output:
               (none)
         */
         void parseRoomContains(string roomName, string targetType);

         /*
            Parse the contents of a Messages object from XML.

            Input:
               Name of Entity we're parsing messages for (string)
               Type of thing we're parsing--"entity" or "class" (string)
               XML Depth--how deeply nested <message> tags will be (int)

            Output:
               (none)
         */
         void parseMessages(string entityName, string targetType, int depth);

         /*
            Parses tags that should be applied to an Entity.

            Input:
               Name of Entity we're parsing messages for (string)
               Type of thing we're parsing--"entity" or "class" (string)
               XML Depth--how deeply nested <message> tags will be (int)

            Output:
               (none)
         */
         void parseEntityTags(string entityName, string targetType, int depth);

         /*
            Parses an <events> section for Entities, Entity classes, and Game.
            Throws an exception if there's an error.

            Input:
               Entity or Entity class name (string -- ignored if targetType = "game")
               Type of thing we're parsing--"entity", "class", or "game" (string)
               Depth in XML tree (int)

            Output:
               (none)
         */
         void parseEvents(string entityName, string targetType, int depth);
         void parseScript(string entityName, string targetType);
         void parseEvent(string entityName, string targetType);

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
         void parseManifestRoom(string className = "room");
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
         void parseEntityMeta(string entityName, string targetType, int depth);

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
         void parseObjectProperties(string name, string targetType, int depth);

         /*
            Parses the creatures section of game.xml.

            Input:
               Tag that closes the creature definition

            Output:
               (none)
         */
         void parseCreatures();
         void parseCreature(string className = "creature");
         void parseCreatureProperties(string name, string targetType, int depth);

         /*
            Parses room definitions in game.xml.

            Input:
               Tag that closes the room definition

            Output:
               (none)
         */
         void parseRooms();
         void parseRoom(string className = "room");
         void parseRoomProperties(string name, string targetType, int depth);

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
            Parses an XML game definition and passes the data off to an
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

