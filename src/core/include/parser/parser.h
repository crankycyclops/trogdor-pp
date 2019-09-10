#ifndef PARSER_H
#define PARSER_H


#include <memory>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <cstdlib>

#include <libxml/xmlreader.h>
#include <boost/lexical_cast.hpp>

#include "../utility.h"
#include "../vocabulary.h"
#include "../entitymap.h"
#include "../entities/player.h"
#include "../lua/luastate.h"
#include "../event/eventlistener.h"

#include "data.h"
#include "entityclass.h"

class Game;


using namespace std;

namespace trogdor { namespace core {


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

         // user defined Entity classes
         entity::EntityClass typeClasses;

         // Global Lua state for the entire game
         std::shared_ptr<LuaState> gameL;

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
               Parse depth in XML file

            Output:
               (none)
         */
         void parseThingAliases(Thing *thing, int depth);
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
            Parses auto-attack settings for a Creature.

            Input:
               Pointer to Creature
               depth in the XML tree (int)

            Output:
               (none)
         */
         void parseCreatureAutoAttack(Creature *creature, int depth);

         /*
            Parses whether or not auto-attack is enabled.

            Input:
               (none)

            Output:
               boolean
         */
         bool parseCreatureAutoAttackEnabled();

         /*
            Parses auto-attack interval.

            Input:
               (none)

            Output:
               number of clock ticks (int)
         */
         int parseCreatureAutoAttackInterval();

         /*
            Parses whether or not auto-attack should repeat as long as both
            Beings are alive and are in the same room.

            Input:
               (none)

            Output:
               boolean
         */
         bool parseCreatureAutoAttackRepeat();

         /*
            Parses a Creature's wandering settings.

            Input:
               Pointer to the Creature whose wandering settings are being configured

            Output:
               (none)
         */
         void parseCreatureWandering(entity::Creature *creature);
         bool parseCreatureWanderingEnabled();
         int parseCreatureWanderingInterval();
         double parseCreatureWanderLust();

         /*
            Parses a Being's respawn settings.

            Input:
               Pointer to the being whose respawn settings are being configured
               Depth in the XML tree

            Output:
               (none)
         */
         void parseBeingRespawn(entity::Being *being, int depth);
         bool parseBeingRespawnEnabled();
         int parseBeingRespawnInterval();
         int parseBeingRespawnLives();

         /*
            Parses a Being's inventory settings.

            Input:
               Pointer to the being whose inventory is being configured
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
            Parses amount of damage a Being does with its bare hands.

            Input:
               (none)

            Output:
               int (damage points)
         */
         int parseBeingDamageBareHands();

         /*
            Parses a Creature's allegiance.

            Input:
               (none)

            Output:
               enum entity::Creature::AllegianceType
         */
         enum entity::Creature::AllegianceType parseCreatureAllegiance();

         /*
            Parses an Object's definition to see if it's takeable or droppable by
            a Being.

            Input:
               (none)

            Output:
               Bool
         */
         bool parseObjectTakeable();
         bool parseObjectDroppable();

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
            Parses whether or not an Object is a weapon.

            Input:
               (none)

            Output:
               (none)
         */
         bool parseObjectWeapon();

         /*
            Parses how much damage an Object does (hit points taken) if it's a
            weapon.

            Input:
               (none)

            Output:
               damage (int)
         */
         int parseObjectDamage();

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
            Parses a single Object in a Room's <contains> section.

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
         void parseEvents(const std::shared_ptr<LuaState> &L, EventListener *triggers, int depth);
         void parseScript(const std::shared_ptr<LuaState> &L);
         void parseEvent(const std::shared_ptr<LuaState> &L, EventListener *triggers);

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
         void parseIntroductionEnabled();
         void parseIntroductionPause();
         void parseIntroductionText();

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
         void parseGameMetaValue(string key);
         void parseEntityMeta(Entity *entity, int depth);
         void parseEntityMetaValue(string key, Entity *entity);

         void parseSynonyms();
         void parseSynonymVerb(string action);

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
         void parseObjectProperties(Object *object, int depth);

         /*
            Parses the creatures section of game.xml.

            Input:
               Tag that closes the creature definition

            Output:
               (none)
         */
         void parseCreatures();
         void parseCreature(string className = "creature");
         void parseCreatureProperties(Creature *creature, int depth);

         /*
            Parses room definitions in game.xml.

            Input:
               Tag that closes the room definition

            Output:
               (none)
         */
         void parseRooms();
         void parseRoom(string className = "room");
         void parseRoomProperties(Room *room, int depth);

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

         inline std::shared_ptr<LuaState> getLuaState() {return gameL;}
         inline event::EventListener *getEventListener() {return eventListener;}
   };
}}


#endif

