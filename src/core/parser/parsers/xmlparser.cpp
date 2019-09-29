#include <memory>

#include "../../include/iostream/nullout.h"
#include "../../include/iostream/nullin.h"
#include "../../include/iostream/placeout.h"
#include "../../include/utility.h"

#include "../../include/parser/parsers/xmlparser.h"
#include "../../include/exception/validationexception.h"
#include "../../include/exception/parseexception.h"

namespace trogdor {


   XMLParser::~XMLParser() {

      if (reader) {
         xmlFreeTextReader(reader);
      }
   }

   /***************************************************************************/

   void XMLParser::parse(string filename) {

      if (reader) {
         xmlFreeTextReader(reader);
         reader = nullptr;
      }

      reader = xmlReaderForFile(filename.c_str(), NULL, XML_PARSE_NOBLANKS);
      if (NULL == reader) {
         throw ParseException("failed to open " + filename + "!\n");
      }

      try {

         // This has to be inside a try block because if the XML file is
         // malformatted (like, for example, there being more than one root
         // element), nextTag() will throw an exception that would otherwise
         // go uncaught.
         if (!nextTag()) {
            throw ParseException(filename + ": empty XML file");
         }

         else if (0 != getTagName().compare("game")) {
            throw ParseException("expected <game>");
         }

         parseGame();
      }

      catch (const Exception &e) {
         throw ParseException(e.what(), filename,
            xmlTextReaderGetParserLineNumber(reader));
      }

      instantiator->instantiate();
   }

   /***************************************************************************/

   void XMLParser::parseGame() {

      bool manifestParsed = false;

      // class definitions and the manifest ALWAYS come first!
      while (!manifestParsed) {

         nextTag();

         if (0 == getTagName().compare("manifest")) {
            parseManifest();
            manifestParsed = true;
         }

         else if (0 == getTagName().compare("classes")) {
            parseClasses();
         }

         else {
            throw ParseException("expected <manifest> or <classes>");
         }
      }

      // parse the remaining sections
      while (nextTag() && 1 == getDepth()) {

         if (0 == getTagName().compare("synonyms")) {
            parseSynonyms();
         }

         else if (0 == getTagName().compare("events")) {
              parseEvents("", PARSE_GAME, 2);
         }

         else if (0 == getTagName().compare("introduction")) {
            parseIntroduction();
         }

         else if (0 == getTagName().compare("meta")) {
            parseGameMeta();
         }

         else if (0 == getTagName().compare("player")) {
            parsePlayer();
         }

         else if (0 == getTagName().compare("objects")) {
            parseObjects();
         }

         else if (0 == getTagName().compare("creatures")) {
            parseCreatures();
         }

         else if (0 == getTagName().compare("rooms")) {
            parseRooms();
         }

         else if (0 != getTagName().compare("classes")) {
            throw ParseException("<classes> must appear before manifest");
         }

         else {
            throw ParseException(string("invalid section <") + getTagName() + ">");
         }
      }

      checkClosingTag("game");
   }

   /***************************************************************************/

   void XMLParser::parseClasses() {

      while (nextTag() && 2 == getDepth()) {

         if (0 == getTagName().compare("rooms")) {
            parseClassesRooms();
         }

         else if (0 == getTagName().compare("objects")) {
            parseClassesObjects();
         }

         else if (0 == getTagName().compare("creatures")) {
            parseClassesCreatures();
         }

         else {
            throw ParseException(string("invalid tag <") + getTagName() + "> in <classes>");
         }
      }

      checkClosingTag("classes");
   }

   /***************************************************************************/

   void XMLParser::parseClassesRooms() {

      while (nextTag() && 3 == getDepth()) {

         if (0 == getTagName().compare("room")) {
            parseClassesRoom();
         }

         else {
            throw ParseException(string("invalid tag <") + getTagName()
               + "> in rooms section of <classes>");
         }
      }

      checkClosingTag("rooms");
   }

   /***************************************************************************/

   void XMLParser::parseClassesRoom() {

      string name = getAttribute("class");

      if (isClassNameReserved(name)) {
         throw ParseException(string("class name '") + name + "' is reserved");
      }

      instantiator->makeEntityClass(name, entity::ENTITY_ROOM);

      parseRoomProperties(name, PARSE_CLASS, 4);
      checkClosingTag("room");
   }

   /***************************************************************************/

   void XMLParser::parseClassesObjects() {

      while (nextTag() && 3 == getDepth()) {

         if (0 == getTagName().compare("object")) {
            parseClassesObject();
         }

         else {
            throw ParseException(string("invalid tag <") + getTagName()
               + "> in objects section of <classes>");
         }
      }

      checkClosingTag("objects");
   }

   /***************************************************************************/

   void XMLParser::parseClassesObject() {

      string name = getAttribute("class");

      if (isClassNameReserved(name)) {
         throw ParseException(string("class name '") + name + "' is reserved");
      }

      instantiator->makeEntityClass(name, entity::ENTITY_OBJECT);

      // Don't pass object! It's been moved and is no longer a valid pointer.
      parseObjectProperties(name, PARSE_CLASS, 4);
      checkClosingTag("object");
   }

   /***************************************************************************/

   void XMLParser::parseClassesCreatures() {

      while (nextTag() && 3 == getDepth()) {

         if (0 == getTagName().compare("creature")) {
            parseClassesCreature();
         }

         else {
            throw ParseException(string("invalid tag <") + getTagName()
               + "> in creatures section of <classes>");
         }
      }

      checkClosingTag("creatures");
   }

   /***************************************************************************/

   void XMLParser::parseClassesCreature() {

      string name = getAttribute("class");

      if (isClassNameReserved(name)) {
         throw ParseException(string("class name '") + name + "' is reserved");
      }

      instantiator->makeEntityClass(name, entity::ENTITY_CREATURE);

      // Don't pass creature! It's been moved and is no longer a valid pointer.
      parseCreatureProperties(name, PARSE_CLASS, 4);
      checkClosingTag("creature");
   }

   /***************************************************************************/

   void XMLParser::parseIntroduction() {

      static unordered_map<string, string> tagToProperty({
         {"enabled", "introduction.enabled"}, {"pause", "introduction.pause"},
         {"text", "introduction.text"}
      });

      while (nextTag() && 2 == getDepth()) {

         string tag = getTagName();

         if (tagToProperty.find(tag) != tagToProperty.end()) {
            string value = parseString();
            instantiator->gameSetter(tagToProperty[tag], value);
            checkClosingTag(tag);
         }

         else {
            throw ParseException(string("invalid tag <") + tag
               + "> in <introduction>");
         }
      }

      checkClosingTag("introduction");
   }

   /***************************************************************************/

   void XMLParser::parseGameMeta() {

      while (nextTag() && 2 == getDepth()) {
         string key = getTagName();
         string value = parseString();
         instantiator->gameSetter("meta", key + ":" + value);
         checkClosingTag(key);
      }

      checkClosingTag("meta");
   }

   /***************************************************************************/

   void XMLParser::parseEntityMeta(string entityName, enum ParseMode mode, int depth) {

      while (nextTag() && depth == getDepth()) {
         string key = getTagName();
         string value = parseString();
         entitySetter(entityName, "meta", key + ":" + value, mode);
         checkClosingTag(key);
      }

      checkClosingTag("meta");
   }

   /***************************************************************************/

   void XMLParser::parseSynonyms() {

      while (nextTag() && 2 == getDepth()) {

         if (0 == getTagName().compare("verb")) {
            string action = getAttribute("action");
            action = trim(action);
            string synonym = parseString();
            instantiator->gameSetter("synonym", synonym + ":" + action);
            checkClosingTag("verb");
         }

         else {
            throw ParseException(string("invalid tag <") + getTagName()
               + "> in <synonyms>");
         }
      }

      checkClosingTag("synonyms");
   }

   /***************************************************************************/

   void XMLParser::parseManifest() {

      while (nextTag() && 2 == getDepth()) {

         if (0 == getTagName().compare("rooms")) {
            parseManifestRooms();
         }

         else if (0 == getTagName().compare("creatures")) {
            parseManifestCreatures();
         }

         else if (0 == getTagName().compare("objects")) {
            parseManifestObjects();
         }

         else {
            throw ParseException(string("invalid tag <") + getTagName()
               + "> in <manifest>");
         }
      }

      checkClosingTag("manifest");
   }

   /***************************************************************************/

   void XMLParser::parseManifestRooms() {

      bool startExists = false;  // true if room "start" exists

      while (nextTag() && 3 == getDepth()) {

         if (0 == getTagName().compare("room")) {
            if (parseManifestRoom()) {
               startExists = true;
            }
         }

         else if (instantiator->entityClassExists(getTagName(), entity::ENTITY_ROOM)) {
            parseManifestRoom(getTagName());
         }

         else {
            throw ParseException(string("invalid tag <") + getTagName()
               + "> in rooms section of <manifest>");
         }
      }

      if (!startExists) {
         throw ParseException("at least one room named \"start\" is required");
      }

      checkClosingTag("rooms");
   }

   /***************************************************************************/

   bool XMLParser::parseManifestRoom(string className) {

      string name = getAttribute("name");
      instantiator->makeEntity(name, entity::ENTITY_ROOM, className);
      checkClosingTag(className);
      return 0 == name.compare("start") ? true : false;
   }

   /***************************************************************************/

   void XMLParser::parseManifestObjects() {

      while (nextTag() && 3 == getDepth()) {

         if (0 == getTagName().compare("object")) {
            parseManifestObject();
         }

         else if (instantiator->entityClassExists(getTagName(), entity::ENTITY_OBJECT)) {
            parseManifestObject(getTagName());
         }

         else {
            throw ParseException(string("<") + getTagName()
               + "> is not an object class in objects section of <manifest>");
         }
      }

      checkClosingTag("objects");
   }

   /***************************************************************************/

   void XMLParser::parseManifestObject(string className) {

      string name = getAttribute("name");
      instantiator->makeEntity(name, entity::ENTITY_OBJECT, className);
      checkClosingTag(className);
   }

   /***************************************************************************/

   void XMLParser::parseManifestCreatures() {

      while (nextTag() && 3 == getDepth()) {

         if (0 == getTagName().compare("creature")) {
            parseManifestCreature();
         }

         else if (instantiator->entityClassExists(getTagName(), entity::ENTITY_CREATURE)) {
            parseManifestCreature(getTagName());
         }

         else {
            throw ParseException(string("invalid tag <") + getTagName()
               + "> in creatures section of <manifest>");
         }
      }

      checkClosingTag("creatures");
   }

   /***************************************************************************/

   void XMLParser::parseManifestCreature(string className) {

      string name = getAttribute("name");
      instantiator->makeEntity(name, entity::ENTITY_CREATURE, className);
      checkClosingTag(className);
   }

   /***************************************************************************/

   void XMLParser::parseMessages(string entityName, enum ParseMode mode, int depth) {

      while (nextTag() && depth == getDepth()) {

         if (0 == getTagName().compare("message")) {

            string messageName = getAttribute("name");
            messageName = trim(messageName);
            string message = parseString();

            setEntityMessage(entityName, messageName, message, mode);
            checkClosingTag("message");
         }

         else {
            throw ParseException("expected <message> in <messages>");
         }
      }

      checkClosingTag("messages");
   }

   /***************************************************************************/

   void XMLParser::parseEntityTags(string entityName, enum ParseMode mode, int depth) {

      while (nextTag() && depth == getDepth()) {

         if (0 == getTagName().compare("tag")) {
            string tag = parseString();
            entitySetter(entityName, "tag", tag, mode);
            checkClosingTag("tag");
         }

         else {
            throw ParseException("expected <tag> in <tags>");
         }
      }

      checkClosingTag("tags");
   }

   /***************************************************************************/

   void XMLParser::parseEvents(string entityName, enum ParseMode mode, int depth) {

      while (nextTag() && depth == getDepth()) {

         if (0 == getTagName().compare("script")) {
            parseScript(entityName, mode);
         }

         else if (0 == getTagName().compare("event")) {
            parseEvent(entityName, mode);
         }

         else {
            throw ParseException(string("invalid tag <") + getTagName()
               + "> in <events>");
         }
      }

      checkClosingTag("events");
   }

   /***************************************************************************/

   void XMLParser::parseScript(string entityName, enum ParseMode mode) {

      string script;
      enum Instantiator::LoadScriptMethod method;

      try {
         script = getAttribute("src");
         method = Instantiator::FILE;
      }

      // There's no src attribute, so we're either parsing an inline script or
      // we're about to encounter an error ;)
      catch (const ParseException &e) {
         script = parseString();
         method = Instantiator::STRING;
      }

      loadScript(mode, script, method, entityName);
      checkClosingTag("script");
   }

   /***************************************************************************/

   void XMLParser::parseEvent(string entityName, enum ParseMode mode) {

      string name = getAttribute("name");
      string function = parseString();

      setEvent(mode, name, function, entityName);
      checkClosingTag("event");
   }

   /***************************************************************************/

   void XMLParser::parseObjects() {

      while (nextTag() && 2 == getDepth()) {

         if (0 == getTagName().compare("object")) {
            parseObject();
         }

         else if (instantiator->entityClassExists(getTagName(), entity::ENTITY_OBJECT)) {
            parseObject(getTagName());
         }

         else {
            throw ParseException(string("invalid tag <") + getTagName()
               + "> in <objects>");
         }
      }

      checkClosingTag("objects");
   }

   /***************************************************************************/

   void XMLParser::parseObject(string className) {

      if (!instantiator->entityExists(getAttribute("name"))) {
         throw ParseException(string("object '") + getAttribute("name")
            + "' was not declared in <manifest>");
      }

      // Make sure entity is an object
      enum entity::EntityType type = instantiator->getEntityType(getAttribute("name"));

      if (entity::ENTITY_OBJECT != type) {
         throw ParseException(string("object type mismatch: '")
            + getAttribute("name") + "' is of type " + Entity::typeToStr(type)
            + ", but was declared in <objects>");
      }

      // Make sure object is the correct class
      string entityClass = instantiator->getEntityClass(getAttribute("name"));

      if (className != entityClass) {
         throw ParseException(string("object type mismatch: '")
            + getAttribute("name") + "' is of class " + entityClass
            + ", but was declared in <objects> to be of class " + className);
      }

      // set the object's default title
      entitySetter(getAttribute("name"), "title",
         string("a ") + getAttribute("name"), PARSE_ENTITY);

      parseObjectProperties(getAttribute("name"), PARSE_ENTITY, 3);
      checkClosingTag(className);
   }

   /***************************************************************************/

   void XMLParser::parseObjectProperties(string name, enum ParseMode mode, int depth) {

      static unordered_map<string, string> tagToProperty({
         {"title", "title"}, {"description", "longDesc"}, {"short", "shortDesc"},
         {"weight", "weight"}, {"takeable", "takeable"}, {"droppable", "droppable"},
         {"damage", "damage"}
      });

      while (nextTag() && depth == getDepth()) {

         string tag = getTagName();

         if (0 == tag.compare("meta")) {
            parseEntityMeta(name, mode, depth + 1);
         }

         else if (0 == tag.compare("messages")) {
            parseMessages(name, mode, depth + 1);
         }

         else if (0 == tag.compare("aliases")) {
            parseThingAliases(name, mode, depth + 1);
         }

         else if (0 == tag.compare("tags")) {
            parseEntityTags(name, mode, depth + 1);
         }

         else if (0 == tag.compare("events")) {
            parseEvents(name, mode, depth + 1);
         }

         else if (tagToProperty.find(tag) != tagToProperty.end()) {
            string value = parseString();
            entitySetter(name, tagToProperty[tag], value, mode);
            checkClosingTag(tag);
         }

         else {
            throw ParseException(string("invalid tag <") + tag
               + "> in object or object class definition");
         }
      }
   }

   /***************************************************************************/

   void XMLParser::parsePlayer() {

      while (nextTag() && 2 == getDepth()) {

         if (0 == getTagName().compare("default")) {
            parseDefaultPlayer();
         }

         else {
            throw ParseException(string("invalid tag <") + getTagName()
               + "> in <player>");
         }
      }

      checkClosingTag("player");
   }

   /***************************************************************************/

   void XMLParser::parseDefaultPlayer() {

      static unordered_map<string, string> tagToProperty({
         {"alive", "alive"}, {"health", "health"}, {"maxhealth", "maxhealth"},
         {"attackable", "attackable"}, {"woundrate", "woundrate"},
         {"damagebarehands", "damagebarehands"}
      });

      while (nextTag() && 3 == getDepth()) {

         string tag = getTagName();

         if (0 == tag.compare("messages")) {
            parseMessages("", PARSE_DEFAULT_PLAYER, 4);
         }

         else if (0 == tag.compare("tags")) {
            parseEntityTags("", PARSE_DEFAULT_PLAYER, 4);
         }

         else if (0 == tag.compare("inventory")) {
            parseBeingInventory("", PARSE_DEFAULT_PLAYER, false);
         }

         else if (0 == tag.compare("respawn")) {
            parseBeingRespawn("", PARSE_DEFAULT_PLAYER, 4);
         }

         else if (0 == tag.compare("attributes")) {
            parseBeingAttributes("", PARSE_DEFAULT_PLAYER);
         }

         else if (tagToProperty.find(tag) != tagToProperty.end()) {
            string value = parseString();
            entitySetter("", tagToProperty[tag], value, PARSE_DEFAULT_PLAYER);
            checkClosingTag(tag);
         }

         else {
            throw ParseException(string("invalid tag <") + tag
               + "> in default section of <player>");
         }
      }

      checkClosingTag("default");
   }

   /***************************************************************************/

   void XMLParser::parseCreatures() {

      while (nextTag() && 2 == getDepth()) {

         if (0 == getTagName().compare("creature")) {
            parseCreature();
         }

         else if (instantiator->entityClassExists(getTagName(), entity::ENTITY_CREATURE)) {
            parseCreature(getTagName());
         }

         else {
            throw ParseException(string("invalid tag <") + getTagName()
               + "> in <creatures>");
         }
      }

      checkClosingTag("creatures");
   }

   /***************************************************************************/

   void XMLParser::parseCreature(string className) {

      if (!instantiator->entityExists(getAttribute("name"))) {
         throw ParseException(string("creature '") + getAttribute("name")
            + "' was not declared in <manifest>");
      }

      // Make sure entity is a creature
      enum entity::EntityType type = instantiator->getEntityType(getAttribute("name"));

      if (entity::ENTITY_CREATURE != type) {
         throw ParseException(string("creature type mismatch: '")
            + getAttribute("name") + "' is of type " + Entity::typeToStr(type)
            + ", but was declared in <creatures>");
      }

      // Make sure creature is the correct class
      string entityClass = instantiator->getEntityClass(getAttribute("name"));

      if (className != entityClass) {
         throw ParseException(string("creature type mismatch: '")
            + getAttribute("name") + "' is of class " + entityClass
            + ", but was declared in <creatures> to be of class " + className);
      }

      // set the creature's default title and parse the rest of its properties
      entitySetter(getAttribute("name"), "title", getAttribute("name"), PARSE_ENTITY);
      parseCreatureProperties(getAttribute("name"), PARSE_ENTITY, 3);
      checkClosingTag(className);
   }

   /***************************************************************************/

   void XMLParser::parseCreatureProperties(string name, enum ParseMode mode, int depth) {

      bool counterAttackParsed = false;

      static unordered_map<string, string> tagToProperty({
         {"title", "title"}, {"description", "longDesc"}, {"short", "shortDesc"},
         {"alive", "alive"}, {"health", "health"}, {"maxhealth", "maxhealth"},
         {"attackable", "attackable"}, {"woundrate", "woundrate"},
         {"damagebarehands", "damagebarehands"}, {"counterattack", "counterattack"},
         {"allegiance", "allegiance"}
      });

      while (nextTag() && depth == getDepth()) {

         string tag = getTagName();

         if (0 == tag.compare("inventory")) {
            parseBeingInventory(name, mode, true);
         }

         else if (0 == tag.compare("respawn")) {
            parseBeingRespawn(name, mode, depth + 1);
         }

         else if (0 == tag.compare("autoattack")) {
            parseCreatureAutoAttack(name, mode, depth + 1);
         }

         else if (0 == tag.compare("wandering")) {
            parseCreatureWandering(name, mode);
         }

         else if (0 == tag.compare("attributes")) {
            parseBeingAttributes(name, mode);
         }

         else if (0 == tag.compare("meta")) {
            parseEntityMeta(name, mode, depth + 1);
         }

         else if (0 == tag.compare("messages")) {
            parseMessages(name, mode, depth + 1);
         }

         else if (0 == tag.compare("aliases")) {
            parseThingAliases(name, mode, depth + 1);
         }

         else if (0 == tag.compare("tags")) {
            parseEntityTags(name, mode, depth + 1);
         }

         else if (0 == tag.compare("events")) {
            parseEvents(name, mode, depth + 1);
         }

         else if (tagToProperty.find(tag) != tagToProperty.end()) {
            string value = parseString();
            entitySetter(name, tagToProperty[tag], value, mode);
            checkClosingTag(tag);
         }

         else {
            throw ParseException(string("invalid tag <") + tag
               + "> in creature definition");
         }
      }

      // Set default counter-attack rules
      if (!counterAttackParsed) {
         entitySetter(name, "counterattack.default", "", mode);
      }
   }

   /***************************************************************************/

   void XMLParser::parseRooms() {

      while (nextTag() && 2 == getDepth()) {

         if (0 == getTagName().compare("room")) {
            parseRoom();
         }

         else if (instantiator->entityClassExists(getTagName(), entity::ENTITY_ROOM)) {
            parseRoom(getTagName());
         }

         else {
            throw ParseException(string("invalid tag <") + getTagName()
               + "> in <rooms>");
         }
      }

      checkClosingTag("rooms");
   }

   /***************************************************************************/

   void XMLParser::parseRoom(string className) {

      if (!instantiator->entityExists(getAttribute("name"))) {
         throw ParseException(string("room '") + getAttribute("name")
            + "' was not declared in <manifest>");
      }

      // Make sure entity is a room
      enum entity::EntityType type = instantiator->getEntityType(getAttribute("name"));

      if (entity::ENTITY_ROOM != type) {
         throw ParseException(string("room type mismatch: '")
            + getAttribute("name") + "' is of type " + Entity::typeToStr(type)
            + ", but was declared in <rooms>");
      }

      // Make sure room is the correct class
      string entityClass = instantiator->getEntityClass(getAttribute("name"));

      if (className != entityClass) {
         throw ParseException(string("room type mismatch: '")
            + getAttribute("name") + "' is of class " + entityClass
            + ", but was declared in <rooms> to be of type " + className);
      }

      // set Room's default title and parse remaining properties
      entitySetter(getAttribute("name"), "title", getAttribute("name"), PARSE_ENTITY);
      parseRoomProperties(getAttribute("name"), PARSE_ENTITY, 3);
      checkClosingTag(className);
   }

   /***************************************************************************/

   void XMLParser::parseRoomProperties(string name, enum ParseMode mode, int depth) {

      static unordered_map<string, string> tagToProperty({
         {"title", "title"}, {"description", "longDesc"}, {"short", "shortDesc"}
      });

      while (nextTag() && depth == getDepth()) {

         string tag = getTagName();

         if (isDirection(tag)) {
            string connection = parseString();
            parseRoomConnection(tag, name, connection, mode);
         }

         else if (0 == tag.compare("contains")) {
            parseRoomContains(name, mode);
         }

         else if (0 == tag.compare("meta")) {
            parseEntityMeta(name, mode, depth + 1);
         }

         else if (0 == tag.compare("messages")) {
            parseMessages(name, mode, depth + 1);
         }

         else if (0 == tag.compare("tags")) {
            parseEntityTags(name, mode, depth + 1);
         }

         else if (0 == tag.compare("events")) {
            parseEvents(name, mode, depth + 1);
         }

         else if (tagToProperty.find(tag) != tagToProperty.end()) {
            string value = parseString();
            entitySetter(name, tagToProperty[tag], value, mode);
            checkClosingTag(tag);
         }

         else {
            throw ParseException(string("invalid tag <") + tag
               + "> in room or class definition");
         }
      }
   }

   /***************************************************************************/

   void XMLParser::parseRoomConnection(string direction, string roomName,
   string connectTo, enum ParseMode mode) {

      entitySetter(roomName, "connection", direction + ":" + connectTo, mode);
      checkClosingTag(direction);
   }

   /***************************************************************************/

   void XMLParser::parseRoomContains(string roomName, enum ParseMode mode) {

      while (nextTag() && 4 == getDepth()) {

         string tag = getTagName();

         if (0 == tag.compare("object") || 0 == tag.compare("creature")) {
               entitySetter(roomName, "contains", parseString(), mode);
         }

         else {
            throw ParseException(string("invalid tag <") + getTagName()
               + "> in <rooms>");
         }

         checkClosingTag(tag);
      }

      checkClosingTag("contains");
   }

   /***************************************************************************/

   void XMLParser::parseCreatureAutoAttack(string creatureName, enum ParseMode mode,
   int depth) {

      static unordered_map<string, string> tagToProperty({
         {"enabled", "autoattack.enabled"}, {"repeat", "autoattack.repeat"},
         {"interval", "autoattack.interval"}
      });

      while (nextTag() && depth == getDepth()) {

         string tag = getTagName();

         if (tagToProperty.find(tag) != tagToProperty.end()) {
            string value = parseString();
            entitySetter(creatureName, tagToProperty[tag], value, mode);
            checkClosingTag(tag);
         }

         else {
            throw ParseException(string("invalid tag <") + getTagName()
               + "> in creature autoattack section");
         }
      }

      checkClosingTag("autoattack");
   }

   /***************************************************************************/

   void XMLParser::parseCreatureWandering(string creatureName, enum ParseMode mode) {

      static unordered_map<string, string> tagToProperty({
         {"enabled", "wandering.enabled"}, {"interval", "wandering.interval"},
         {"wanderlust", "wandering.wanderlust"}
      });

      while (nextTag() && 4 == getDepth()) {

         string tag = getTagName();

         if (tagToProperty.find(tag) != tagToProperty.end()) {
            string value = parseString();
            entitySetter(creatureName, tagToProperty[tag], value, mode);
            checkClosingTag(tag);
         }

         else {
            throw ParseException(string("invalid tag <") + tag
               + "> in creature wandering settings");
         }
      }

      checkClosingTag("wandering");
   }

   /***************************************************************************/

   void XMLParser::parseBeingRespawn(string beingName, enum ParseMode mode, int depth) {

      static unordered_map<string, string> tagToProperty({
         {"enabled", "respawn.enabled"}, {"interval", "respawn.interval"},
         {"lives", "respawn.lives"}
      });

      while (nextTag() && depth == getDepth()) {

         string tag = getTagName();

         if (tagToProperty.find(tag) != tagToProperty.end()) {
            string value = parseString();
            entitySetter(beingName, tagToProperty[tag], value, mode);
            checkClosingTag(tag);
         }

         else {
            throw ParseException(string("invalid tag <") + tag
               + "> in being respawn section");
         }
      }

      checkClosingTag("respawn");
   }

   /***************************************************************************/

   void XMLParser::parseBeingInventory(string beingName, enum ParseMode mode,
   bool allowObjects) {

      static unordered_map<string, string> tagToProperty({
         {"weight", "inventory.weight"}, {"object", "inventory.object"}
      });

      while (nextTag() && 4 == getDepth()) {

         string tag = getTagName();

         if (tagToProperty.find(tag) != tagToProperty.end()) {
            string value = parseString();
            entitySetter(beingName, tagToProperty[tag], value, mode);
            checkClosingTag(tag);
         }

         else {
            throw ParseException(string("invalid tag <") + tag
               + "> in inventory settings");
         }
      }

      checkClosingTag("inventory");
   }

   /***************************************************************************/

   void XMLParser::parseBeingAttributes(string beingName, enum ParseMode mode) {

      while (nextTag() && 4 == getDepth()) {

        string tag = getTagName();

         // TODO: should I allow any tag here? Custom attribute values are
         // something I'd like to support in the future, and it would make error
         // checking much simpler. At worst, custom values would just be ignored
         // during gameplay until they're supported.
         if (
            0 == tag.compare("strength") ||
            0 == tag.compare("dexterity") ||
            0 == tag.compare("intelligence")
         ) {
            string value = parseString();
            entitySetter(beingName, string("attribute"), tag + ":" + value, mode);
            checkClosingTag(tag);
         }

         else {
            throw ParseException(string("invalid tag <") + tag
               + "> in default player's inventory settings");
         }
      }

      checkClosingTag("attributes");
   }

   /***************************************************************************/

   void XMLParser::parseThingAliases(string entityName, enum ParseMode mode, int depth) {

      while (nextTag() && depth == getDepth()) {

         if (0 == getTagName().compare("alias")) {
            string alias = parseString();
            entitySetter(entityName, "alias", alias, mode);
            checkClosingTag("alias");
         }

         else {
            throw ParseException(string("invalid tag <") + getTagName()
               + "> in <aliases>");
         }
      }

      checkClosingTag("aliases");
   }

   /***************************************************************************/

   string XMLParser::parseString() {

      string value = getNodeValue();
      value = trim(value);

      if (value.length() > 0) {
         return value;
      }

      else {
         throw ParseException("Tag requires a value");
      }
   }

   /***************************************************************************/

   bool XMLParser::nextTag() {

      int status = xmlTextReaderRead(reader);

      // skip comments
      while (XML_COMMENT_NODE == xmlTextReaderNodeType(reader) && status > 0) {
         status = xmlTextReaderRead(reader);
      }

      // we've reached a closing tag, so signal subsequent call to
      // checkClosingTag()
      if (XML_ELEMENT_DECL == xmlTextReaderNodeType(reader)) {
         lastClosedTag = getTagName();
      }

      // we're supposed to be getting an opening tag
      else if (XML_ELEMENT_NODE != xmlTextReaderNodeType(reader)) {
         throw ParseException("expected opening tag");
      }

      if (status < 0) {
         throw ParseException("unknown error when getting next tag");
      }

      // we've reached the end of the XML file
      else if (0 == status) {
         return false;
      }

      // we have a tag to parse
      else {
         return true;
      }
   }

   /***************************************************************************/

   string XMLParser::getAttribute(const char *name) {

      const char *attr;

      attr = (const char *)xmlTextReaderGetAttribute(reader, (xmlChar *)name);

      if (!attr) {
         throw ParseException(string("missing attribute '") + name);
      }

      return attr;
   }

   /***************************************************************************/

   const char *XMLParser::getNodeValue() {

      // get the corresponding #text node for the value
      if (xmlTextReaderRead(reader) < 0) {
         throw ParseException("Unknown error when getting current node's value");
      }

      else if (0 != strcmp("#text", (const char *)xmlTextReaderConstName(reader))) {
         throw ParseException("tag must have a value");
      }

      return (const char *)xmlTextReaderValue(reader);
   }

   /***************************************************************************/

   void XMLParser::checkClosingTag(string tag) {

      // check to see if nextTag() encountered a closing tag
      if (lastClosedTag.length() > 0) {

         if (0 != lastClosedTag.compare(tag)) {
            throw ParseException(string("expected closing </") + tag + ">");
         }

         lastClosedTag = "";
         return;
      }

      // TODO: skip over XML comments (right now, we'll get an error!)

      if (xmlTextReaderRead(reader) < 0) {
         throw ParseException(string("Unknown error when checking closing tag </") + tag + ">");
      }

      else if (XML_ELEMENT_DECL != xmlTextReaderNodeType(reader) ||
      0 != tag.compare(strToLower((const char *)xmlTextReaderConstName(reader)))) {
         throw ParseException(string("missing closing </") + tag + ">");
      }
   }
}

