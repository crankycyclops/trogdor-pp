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
#include "vocabulary.h"
#include "entitymap.h"
#include "entities/player.h"
#include "luastate.h"
#include "eventlistener.h"

class Game;


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

         // When nextTag() encounters a closing tag, it's stored here.  Later,
         // if this has a value, checkClosingTag() will use it instead of
         // walking the XML tree.
         string lastClosedTag;

         // Entities must be instantiated with a reference to their Game
         Game *game;

         // Player object representing default settings for all new players
         entity::Player *defaultPlayer;

         // hash tables for various types of entities
         // Note: the logical conclusion of having a hierarchical mapping of
         // object types is that no object of any type can share the same name!
         // This can be worked around via aliases :)
         entity::EntityMap    entities;
         entity::PlaceMap     places;
         entity::ThingMap     things;
         entity::RoomMap      rooms;
         entity::BeingMap     beings;
         entity::ItemMap      items;
         entity::CreatureMap  creatures;
         entity::ObjectMap    objects;

         // Global Lua state for the entire game
         LuaState *gameL;

         // Global EventListener for the entire game
         event::EventListener *eventListener;

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

         /*
            Parses an Entity's title.

            Input:
               (none)

            Output:
               The title (string)
         */
         string parseEntityTitle();

         /*
            Parses an Entity's long and short descriptions.

            Input:
               (none)

            Output:
               description (string)
         */
         string parseEntityLongDescription();
         string parseEntityShortDescription();

         /*
            Parses a list of aliases, alternate identifiers for Things.

            Input:
               Pointer to Thing

            Output:
               (none)
         */
         void parseThingAliases(Thing *thing);
         string parseThingAlias();  // returns a single alias

         /*
            Parses whether or not creature should automatically retaliate against
            an attack.

            Input:
               (none)

            Output:
               bool
         */
         bool parseCreatureCounterAttack();

         /*
            Parses a Being's inventory settings.

            Input:
               Reference to the being whose inventory is being configured
               Whether or not to allow a set of objects initialized in game.xml

            Output:
               (none)
         */
         void parseBeingInventory(entity::Being *being, bool allowObjects);
         void parseBeingInventoryObject(Being *being);
         int parseBeingInventoryWeight();

         /*
            Parses a Being's Attributes.

            Input:
               Pointer to being whose attributes are being set

            Output:
               (none)
         */
         void parseBeingAttributes(entity::Being *being);
         int parseBeingAttributesStrength();
         int parseBeingAttributesDexterity();
         int parseBeingAttributesIntelligence();

         /*
            Parses whether or not a Being starts out being alive.

            Input:
               (none)

            Output:
               (none)
         */
         bool parseBeingAlive();

         /*
            Parses a Being's initial health.

            Input:
               (none)

            Output:
               boolean (alive or dead)
         */
         int parseBeingHealth();

         /*
            Parses a Being's maximum health (0 for immortal.)

            Input:
               (none)

            Output:
               Being's health (int)
         */
         int parseBeingMaxHealth();

         /*
            Parses a Being's woundRate setting, which is a factor that influences how
            likely a Being is to be hit during an attack.

            Input:
               (none)

            Output:
               Being's max health (int)
         */
         double parseBeingWoundRate();

         /*
            Parses whether or not players, by default, are attackable.

            Input:
               (none)

            Output:
               bool
         */
         bool parseBeingAttackable();

         /*
            Parses a Creature's allegiance.

            Input:
               (none)

            Output:
               enum entity::Creature::AllegianceType
         */
         enum entity::Creature::AllegianceType parseCreatureAllegiance();

         /*
            Parses an Item's definition to see if it's takeable or droppable by
            a Being.

            Input:
               (none)

            Output:
               Bool
         */
         bool parseItemTakeable();
         bool parseItemDroppable();

         /*
            Parses an Object's weight (how much space it takes up in a Being's
            inventory.

            Input:
               (none)

            Output:
               weight (int)
         */
         int parseObjectWeight();

         /*
            Parses whether or not an Item is a weapon.

            Input:
               (none)

            Output:
               (none)
         */
         bool parseItemWeapon();

         /*
            Parses how much damage an object does (hit points taken) if it's a
            weapon.

            Input:
               (none)

            Output:
               damage (int)
         */
         int parseItemDamage();

         /*
            Takes tags such as <north>, <south>, etc. in a room definition and
            forges that connection.

            Input:
               direction (string)
               Pointer to room where connection should be made (Room *)
               Name of room that we want to connect to (string)

            Output:
               (none)
         */
         void parseRoomConnection(string direction, Room *room, string connectTo);

         /*
            Parses a Room's version of the <contains> section.

            Input:
               Pointer to Room in which we're inserting the object

            Output:
               (none)
         */
         void parseRoomContains(Room *room);

         /*
            Parses a single item in a Room's <contains> section.

            Input:
               Tag name (right now, could be "creature" or "object")

            Output:
               (none)
         */
         Thing *parseRoomContainsThing(string tag);

         /*
            Parse the contents of a Messages object from XML.

            Input:
               depth -- how deeply nested <message> tags will be (int)

            Output:
               Messages object containing the parsed messages
         */
         Messages *parseMessages(int depth);

         /*
            Called by parseMessages() to parse a single message from the XML.
            Stores the result in the Message object pointed to by m.

            Input:
               Messages object that will contain the message

            Output:
               (none)
         */
         void parseMessage(Messages *m);

         /*
            Parses an <events> section and populates the given LuaState and
            EventListener objects.  Throws an exception if there's an error.

            Input:
               Pointer to LuaState
               Pointer to EventListener
               Depth in XML tree (int)

            Output:
               (none)
         */
         void parseEvents(LuaState *L, EventListener *triggers, int depth);
         void parseScript(LuaState *L);
         void parseEvent(LuaState *L, EventListener *triggers);

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
         bool parseManifestRoom();  // returns true if room's name is "start"
         void parseManifestCreatures();
         void parseManifestCreature();
         void parseManifestObjects();
         void parseManifestObject();

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
               (none)

            Output:
               (none)
         */
         void parseObjects();
         void parseObject();

         /*
            Parses the creatures section of game.xml.

            Input:
               (none)

            Output:
               (none)
         */
         void parseCreatures();
         void parseCreature();

         /*
            Parses room definitions in game.xml.

            Input:
               (none)

            Output:
               (none)
         */
         void parseRooms();
         void parseRoom();

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
            Constructor for the Parser class.  Takes as input the filename of
            the game XML file and opens the file for reading.  If the specified
            file cannot be opened, an exception is thrown with an error message.
         */
         Parser(Game *g, string gameFile);

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

         // TODO: should the copy go deeper?
         inline entity::EntityMap getEntities() {return entities;}
         inline entity::PlaceMap getPlaces() {return places;}
         inline entity::ThingMap getThings() {return things;}
         inline entity::RoomMap getRooms() {return rooms;}
         inline entity::BeingMap getBeings() {return beings;}
         inline entity::CreatureMap getCreatures() {return creatures;}
         inline entity::ItemMap getItems() {return items;}
         inline entity::ObjectMap getObjects() {return objects;}

         inline entity::Player *getDefaultPlayer() {return defaultPlayer;}

         inline LuaState *getLuaState() {return gameL;}
         inline event::EventListener *getEventListener() {return eventListener;}
   };
}


#endif

