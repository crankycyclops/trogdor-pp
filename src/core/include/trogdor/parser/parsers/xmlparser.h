#ifndef XMLPARSER_H
#define XMLPARSER_H


#include <memory>
#include <optional>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unordered_map>
#include <unordered_set>

#include <libxml/xmlreader.h>

#include <trogdor/utility.h>
#include <trogdor/vocabulary.h>
#include <trogdor/entities/player.h>
#include <trogdor/lua/luastate.h>

#include <trogdor/parser/parser.h>


namespace trogdor {

   /*
      Parses an XML game definition.
   */
   class XMLParser: public Parser {

      private:

         // Keeping a copy of the XML file's path will help later when we try to
         // locate Lua script files using relative paths
         std::string gamefilePath;

         // Whenever an entity is explicitly declared, it's recorded here. We
         // check this later to see if any entities are referenced that weren't
         // declared (i.e. Room "start" has an eastern connection to room "cave",
         // which was never declared) and throw an error if so
         struct DeclaredEntity {
            int lineno;
            std::string name;
            std::string className;
            entity::EntityType type;
         };

         // handles the actual XML parsing
         xmlTextReaderPtr reader;

         // When nextTag() encounters a closing tag, it's stored here.  Later,
         // if this has a value, checkClosingTag() will use it instead of
         // walking the XML tree.
         std::string lastClosedTag;

         // Keeps track of Entities as they're declared
         std::unordered_map<std::string, DeclaredEntity> declaredEntities;

         // Keeps track of Entity classes, defined in <classes>
         std::unordered_map<std::string, entity::EntityType> declaredEntityClasses;

         // Any time an entity is referenced by another (i.e. a room connection
         // or an item in a Creature's inventory) and hasn't been declared, that
         // reference is recorded here. When parsing is complete, we'll perform
         // a check for unresolved references, and if any are found, an error
         // will be thrown.
         std::unordered_map<

            // The referenced entity's name
            std::string,

            std::pair<

               // AST Node where the entity is defined (this way, we can modify
               // it in place later if necessary)
               std::shared_ptr<ASTNode>,

               // All the line numbers where the entity is referenced
               std::vector<int>
            >

         > unresolvedEntityReferences;

         // Keeps track of custom directions and direction synonyms
         std::unordered_set<std::string> customDirections;

         /*
            Declare an entity. If the entity has already been previously
            declared, an error will be thrown. If the specified class hasn't
            been defined for entities of its type, then an error will also be
            thrown.

            Input:
               Entity name (std::string)
               Entity class name (std::string)
               Entity type (entity::EntityType)
               Entity's plural name, for Resource type only (std::optional<std::string>)
               Line number (int)

            Output:
               (none)
         */
         void declareEntity(
            std::string name,
            std::string className,
            entity::EntityType type,
            std::optional<std::string> plural,
            int lineno
         );

         /*
            Records a reference to an Entity that hasn't been declared yet.

            Input:
               Entity name (std::string)
               Entity class name (std::string)
               Entity type (entity::EntityType)
               Line number (int)

            Output:
               Reference to the resulting ASTNode (std::shared_ptr<ASTNode> &)
         */
         void setUnresolvedEntityReference(
            std::string name,
            std::string className,
            entity::EntityType type,
            int lineno
         );

         /*
            Updates the AST with a define entity operation.

            Input:
               Entity name (std::string)
               Entity class name (std::string)
               Entity type (entity::EntityType)
               Entity's plural name for Resource type only (std::optional<std::string>)
               Line number (int)

            Output:
               Shared pointer to the resulting ASTNode (std::shared_ptr<ASTNode>)
         */
         std::shared_ptr<ASTNode> insertDefineEntityOperation(
            std::string name,
            std::string className,
            entity::EntityType type,
            int lineno,
            std::optional<std::string> plural = std::nullopt
         );

         /*
            Returns true if the specified Entity class was declared and is of
            the specified type and false if not.

            Input:
               (none)

            Output:
               Whether or not the Entity class of the specified type exists (bool)
         */
         inline bool entityClassDeclared(std::string name, entity::EntityType type) {

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
         inline std::string getTagName() {

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
               ParseException is thrown if the next tag isn't opening or there's
               a parse error.
         */
         bool nextTag();

         /*
            Gets the value of the specified attribute.

            Input:
               Attribute name (const C string)

            Output:
               Attribute value (std::string)

            Exceptions:
               ParseException is thrown if the attribute isn't set.
         */
         std::string getAttribute(const char *name);

         /*
            Gets the value of the specified attribute. Since the attribute is
            optional, this does not throw an exception if the attribute isn't
            set but returns std::nullopt instead.

            Input:
               Attribute name (const C string)

            Output:
               Attribute value if it's set and std::nullopt if it's not
               (std::optional<std::string>)
         */
         std::optional<std::string> getOptionalAttribute(const char *name);

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
         void checkClosingTag(std::string tag);

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
         std::string parseString();

         /*
            Parses a list of aliases, alternate identifiers for Things.

            Input:
               Name of Thing (std::string)
               Type of thing we're parsing--one of "entity" or "class" (std::string)
               Parse depth in XML file

            Output:
               (none)
         */
         void parseThingAliases(std::string entityName, std::string targetType, int depth);

         /*
            Parses auto-attack settings for a Creature.

            Input:
               Name of Creature (std::string)
               Type of thing we're parsing--"entity" or "class" (std::string)
               depth in the XML tree (int)

            Output:
               (none)
         */
         void parseCreatureAutoAttack(std::string creatureName, std::string targetType,
         int depth);

         /*
            Parses a Creature's wandering settings.

            Input:
               Name of Creature (std::string)
               Type of thing we're parsing--"entity" or "class" (std::string)

            Output:
               (none)
         */
         void parseCreatureWandering(std::string creatureName, std::string targetType);

         /*
            Parses a Being's respawn settings.

            Input:
               Name of being whose respawn settings are being configured
               Type of thing we're parsing--"entity" or "class" (std::string)
               Depth in the XML tree

            Output:
               (none)
         */
         void parseBeingRespawn(std::string beingName, std::string targetType, int depth);

         /*
            Parses a Being's inventory settings.

            Input:
               Name of being whose respawn settings are being configured
               Type of thing we're parsing--"entity", "class", or "defaultPlayer"
               Whether or not to allow a set of objects initialized in game.xml

            Output:
               (none)
         */
         void parseBeingInventory(std::string beingName, std::string targetType,
         bool allowObjects);

         /*
            Parses a Being's Attributes.

            Input:
               Name of Being whose attributes are being set
               Type of thing we're parsing--"entity" or "class" (std::string)

            Output:
               (none)
         */
         void parseBeingAttributes(std::string beingName, std::string targetType);

         /*
            Takes tags such as <north>, <south>, etc. in a room definition and
            forges that connection.

            Input:
               direction (std::string)
               Name of room where the connection should be made (std::string)
               Name of room that we want to connect to (std::string)
               Type of thing we're parsing--"entity" or "class" (std::string)

            Output:
               (none)
         */
         void parseRoomConnection(std::string direction, std::string roomName,
         std::string connectTo, std::string targetType);

         /*
            Parses a Room's version of the <contains> section.

            Input:
               Name of room into which we're inserting the object (std::string)
               Type of thing we're parsing--"entity" or "class" (std::string)

            Output:
               (none)
         */
         void parseRoomContains(std::string roomName, std::string targetType);

         /*
            Parse the contents of a Messages object from XML.

            Input:
               Name of Entity we're parsing messages for (std::string)
               Type of thing we're parsing--"entity" or "class" (std::string)
               XML Depth--how deeply nested <message> tags will be (int)

            Output:
               (none)
         */
         void parseMessages(std::string entityName, std::string targetType, int depth);

         /*
            Parses tags that should be applied to an Entity.

            Input:
               Name of Entity we're parsing messages for (std::string)
               Type of thing we're parsing--"entity" or "class" (std::string)
               XML Depth--how deeply nested <message> tags will be (int)

            Output:
               (none)
         */
         void parseEntityTags(std::string entityName, std::string targetType, int depth);

         /*
            Parses an <events> section for Entities, Entity classes, and Game.
            Throws an exception if there's an error.

            Input:
               Entity or Entity class name (std::string -- ignored if targetType = "game")
               Type of thing we're parsing--"entity", "class", or "game" (std::string)
               Depth in XML tree (int)

            Output:
               (none)
         */
         void parseEvents(std::string entityName, std::string targetType, int depth);
         void parseScript(std::string entityName, std::string targetType);
         void parseEvent(std::string entityName, std::string targetType);

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
         void parseClassesResources();
         void parseClassesResource();

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
            Parses meta data for entities or for the game.

            Input:
               (none)

            Output:
               (none)
         */
         void parseGameMeta();
         void parseEntityMeta(std::string entityName, std::string targetType, int depth);

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
               The object's class (std::string)

            Output:
               (none)
         */
         void parseObjects();
         void parseObject(std::string className = "object");
         void parseObjectProperties(std::string name, std::string targetType, int depth);

         /*
            Parses the creatures section of game.xml.

            Input:
               Tag that closes the creature definition

            Output:
               (none)
         */
         void parseCreatures();
         void parseCreature(std::string className = "creature");
         void parseCreatureProperties(std::string name, std::string targetType, int depth);

         /*
            Parses room definitions in game.xml.

            Input:
               Tag that closes the room definition

            Output:
               (none)
         */
         void parseRooms();
         void parseRoom(std::string className = "room");
         void parseRoomProperties(std::string name, std::string targetType, int depth);

         /*
            Parses resource definitions in game.xml.

            Input:
               Tag that closes the room definition

            Output:
               (none)
         */
         void parseResources();
         void parseResource(std::string className = "resource");
         void parseResourceProperties(std::string name, std::string targetType, int depth);

         /*
            Parses a tangible entity's resource allocation section.

            Input:
               The entity name

            Output:
               (none)
         */
         void parseTangibleResources(std::string entityName, int depth);

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
         inline XMLParser(
            std::unique_ptr<Instantiator> i,
            const Vocabulary &v,
            const Trogerr &e
         ): Parser(std::move(i), v, e) {

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
               Filename where the game definition is saved (std::string)

            Output:
               (none)
         */
         virtual void parse(std::string filename);
   };
}


#endif
