#include <memory>

#include "../include/parser/parser.h"
#include "../include/iostream/nullout.h"
#include "../include/iostream/nullin.h"
#include "../include/iostream/placeout.h"


using namespace std;

namespace trogdor { namespace core {


   Parser::Parser(std::unique_ptr<Instantiator> i, string gameFile) {

      instantiator = std::move(i);

      filename = gameFile;

      reader = xmlReaderForFile(gameFile.c_str(), NULL, XML_PARSE_NOBLANKS);
      if (NULL == reader) {
         throw "failed to open " + gameFile + "!\n";
      }
   }

   /***************************************************************************/

   Parser::~Parser() {

      xmlFreeTextReader(reader);
   }

   /***************************************************************************/

   void Parser::parse() {

      if (!nextTag()) {
         throw filename + ": empty XML file";
      }

      if (0 != getTagName().compare("game")) {
         throw filename + ": expecting <game>";
      }

      parseGame();
      instantiator->instantiate();
   }

   /***************************************************************************/

   void Parser::parseGame() {

      stringstream s;
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
            s << filename << ": expected <manifest> or <classes> (line " <<
               xmlTextReaderGetParserLineNumber(reader) << ")";
            throw s.str();
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
            s << filename << ": <classes> must appear before manifest" <<
               "(line " << xmlTextReaderGetParserLineNumber(reader) << ")";
            throw s.str();
         }

         else {
            s << filename << ": invalid section <" << getTagName() <<
               "> (line " << xmlTextReaderGetParserLineNumber(reader) << ")";
            throw s.str();
         }
      }

      checkClosingTag("game");
   }

   /***************************************************************************/

   void Parser::parseClasses() {

      stringstream s;

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
            s << filename << ": invalid tag <" << getTagName() << "> in "
               << "<classes> section (line " << xmlTextReaderGetParserLineNumber(reader)
               << ")";
            throw s.str();
         }
      }

      checkClosingTag("classes");
   }

   /***************************************************************************/

   void Parser::parseClassesRooms() {

      stringstream s;

      while (nextTag() && 3 == getDepth()) {

         if (0 == getTagName().compare("room")) {
            parseClassesRoom();
         }

         else {
            s << filename << ": invalid tag <" << getTagName() << "> in "
               << "rooms section of <classes> (line "
               << xmlTextReaderGetParserLineNumber(reader) << ")";
            throw s.str();
         }
      }

      checkClosingTag("rooms");
   }

   /***************************************************************************/

   void Parser::parseClassesRoom() {

      string name = getAttribute("class");
      stringstream s;

      if (isClassNameReserved(name)) {
         s << filename << ": class name '" << name << "' is reserved (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      try {
         instantiator->makeEntityClass(name, entity::ENTITY_ROOM);
      }

      catch (bool e) {
         s << filename << ": room class '" << name << "' already defined (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      catch (string e) {
         s << filename << ": " << e << " (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      parseRoomProperties(name, PARSE_CLASS, 4);
      checkClosingTag("room");
   }

   /***************************************************************************/

   void Parser::parseClassesObjects() {

      stringstream s;

      while (nextTag() && 3 == getDepth()) {

         if (0 == getTagName().compare("object")) {
            parseClassesObject();
         }

         else {
            s << filename << ": invalid tag <" << getTagName() << "> in "
               << "objects section of <classes> (line "
               << xmlTextReaderGetParserLineNumber(reader) << ")";
            throw s.str();
         }
      }

      checkClosingTag("objects");
   }

   /***************************************************************************/

   void Parser::parseClassesObject() {

      string name = getAttribute("class");
      stringstream s;

      if (isClassNameReserved(name)) {
         s << filename << ": class name '" << name << "' is reserved (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      try {
         instantiator->makeEntityClass(name, entity::ENTITY_OBJECT);
      }

      catch (bool e) {
            s << filename << ": object class '" << name << "' already defined (line "
               << xmlTextReaderGetParserLineNumber(reader) << ")";
            throw s.str();
      }

      catch (string error) {
         s << filename << ": " << error << " (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      // Don't pass object! It's been moved and is no longer a valid pointer.
      parseObjectProperties(name, PARSE_CLASS, 4);
      checkClosingTag("object");
   }

   /***************************************************************************/

   void Parser::parseClassesCreatures() {

      stringstream s;

      while (nextTag() && 3 == getDepth()) {

         if (0 == getTagName().compare("creature")) {
            parseClassesCreature();
         }

         else {
            s << filename << ": invalid tag <" << getTagName() << "> in "
               << "creatures section of <classes> (line "
               << xmlTextReaderGetParserLineNumber(reader) << ")";
            throw s.str();
         }
      }

      checkClosingTag("creatures");
   }

   /***************************************************************************/

   void Parser::parseClassesCreature() {

      string name = getAttribute("class");
      stringstream s;

      if (isClassNameReserved(name)) {
         s << filename << ": class name '" << name << "' is reserved (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      try {
         instantiator->makeEntityClass(name, entity::ENTITY_CREATURE);
      }

      catch (bool e) {
            s << filename << ": creature class '" << name << "' already defined (line "
               << xmlTextReaderGetParserLineNumber(reader) << ")";
            throw s.str();
      }

      catch (string e) {
         s << filename << ": " << e << " (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      // Don't pass creature! It's been moved and is no longer a valid pointer.
      parseCreatureProperties(name, PARSE_CLASS, 4);
      checkClosingTag("creature");
   }

   /***************************************************************************/

   void Parser::parseIntroduction() {

      stringstream s;
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
            s << filename << ": invalid tag <" << tag <<
               "> in <introduction> section (line " <<
               xmlTextReaderGetParserLineNumber(reader) << ")";
            throw s.str();
         }
      }

      checkClosingTag("introduction");
   }

   /***************************************************************************/

   void Parser::parseGameMeta() {

      while (nextTag() && 2 == getDepth()) {
         string key = getTagName();
         string value = parseString();
         instantiator->gameSetter("meta", key + ":" + value);
         checkClosingTag(key);
      }

      checkClosingTag("meta");
   }

   /***************************************************************************/

   void Parser::parseEntityMeta(string entityName, enum ParseMode mode, int depth) {

      while (nextTag() && depth == getDepth()) {
         string key = getTagName();
         string value = parseString();
         entitySetter(entityName, "meta", key + ":" + value, mode);
         checkClosingTag(key);
      }

      checkClosingTag("meta");
   }

   /***************************************************************************/

   void Parser::parseSynonyms() {

      stringstream s;

      while (nextTag() && 2 == getDepth()) {

         if (0 == getTagName().compare("verb")) {
            string action = getAttribute("action");
            action = trim(action);
            string synonym = parseString();
            instantiator->gameSetter("synonym", synonym + ":" + action);
            checkClosingTag("verb");
         }

         else {
            s << filename << ": invalid tag <" << getTagName() <<
               "> in <synonyms> section (line " <<
               xmlTextReaderGetParserLineNumber(reader) << ")";
            throw s.str();
         }
      }

      checkClosingTag("synonyms");
   }

   /***************************************************************************/

   void Parser::parseManifest() {

      stringstream s;

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
            s << filename << ": invalid tag <" << getTagName() << "> in "
               << "manifest (line " << xmlTextReaderGetParserLineNumber(reader)
               << ")";
            throw s.str();
         }
      }

      checkClosingTag("manifest");
   }

   /***************************************************************************/

   void Parser::parseManifestRooms() {

      stringstream s;
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
            s << filename << ": invalid tag <" << getTagName() << "> in "
               << "rooms section of the manifest (line "
               << xmlTextReaderGetParserLineNumber(reader) << ")";
            throw s.str();
         }
      }

      if (!startExists) {
         throw filename + ": at least one room with name \"start\" is required";
      }

      checkClosingTag("rooms");
   }

   /***************************************************************************/

   bool Parser::parseManifestRoom(string className) {

      string name = getAttribute("name");
      instantiator->makeEntity(name, entity::ENTITY_ROOM, className);
      checkClosingTag(className);
      return 0 == name.compare("start") ? true : false;
   }

   /***************************************************************************/

   void Parser::parseManifestObjects() {

      stringstream s;

      while (nextTag() && 3 == getDepth()) {

         if (0 == getTagName().compare("object")) {
            parseManifestObject();
         }

         else if (instantiator->entityClassExists(getTagName(), entity::ENTITY_OBJECT)) {
            parseManifestObject(getTagName());
         }

         else {
            s << filename << ": <" << getTagName() << "> is not an object "
               << "class in objects section of the manifest (line "
               << xmlTextReaderGetParserLineNumber(reader) << ")";
            throw s.str();
         }
      }

      checkClosingTag("objects");
   }

   /***************************************************************************/

   void Parser::parseManifestObject(string className) {

      string name = getAttribute("name");
      instantiator->makeEntity(name, entity::ENTITY_OBJECT, className);
      checkClosingTag(className);
   }

   /***************************************************************************/

   void Parser::parseManifestCreatures() {

      stringstream s;

      while (nextTag() && 3 == getDepth()) {

         if (0 == getTagName().compare("creature")) {
            parseManifestCreature();
         }

         else if (instantiator->entityClassExists(getTagName(), entity::ENTITY_CREATURE)) {
            parseManifestCreature(getTagName());
         }

         else {
            s << filename << ": invalid tag <" << getTagName() << "> in "
               << "creatures section of the manifest (line "
               << xmlTextReaderGetParserLineNumber(reader) << ")";
            throw s.str();
         }
      }

      checkClosingTag("creatures");
   }

   /***************************************************************************/

   void Parser::parseManifestCreature(string className) {

      string name = getAttribute("name");
      instantiator->makeEntity(name, entity::ENTITY_CREATURE, className);
      checkClosingTag(className);
   }

   /***************************************************************************/

   void Parser::parseMessages(string entityName, enum ParseMode mode, int depth) {

      stringstream s;

      while (nextTag() && depth == getDepth()) {

         if (0 == getTagName().compare("message")) {

            try {

               string messageName = getAttribute("name");
               messageName = trim(messageName);
               string message = parseString();

               setEntityMessage(entityName, messageName, message, mode);
               checkClosingTag("message");
            }

            catch (string error) {
               s << filename << ": " << error << " (line "
                  << xmlTextReaderGetParserLineNumber(reader) << ")";
               throw s.str();
            }
         }

         else {
            s << filename << ": invalid tag <" << getTagName() << "> in "
               << "messages section: expected <message> (line "
               << xmlTextReaderGetParserLineNumber(reader) << ")";
            throw s.str();
         }
      }

      checkClosingTag("messages");
   }

   /***************************************************************************/

   void Parser::parseEvents(string entityName, enum ParseMode mode, int depth) {

      stringstream s;

      try {

         while (nextTag() && depth == getDepth()) {

            if (0 == getTagName().compare("script")) {
               parseScript(entityName, mode);
            }

            else if (0 == getTagName().compare("event")) {
               parseEvent(entityName, mode);
            }

            else {
               s << filename << ": invalid tag <" << getTagName() << "> in "
                  << "<events> (line " << xmlTextReaderGetParserLineNumber(reader)
                  << ")";
               throw s.str();
            }
         }
      }

      catch (string error) {
         s << filename << ": " << error << " (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      checkClosingTag("events");
   }

   /***************************************************************************/

   void Parser::parseScript(string entityName, enum ParseMode mode) {

      stringstream s;
      string script;
      enum Instantiator::LoadScriptMethod method;

      try {

         try {
            script = getAttribute("src");
            method = Instantiator::FILE;
         }

         catch (string error) {
            script = parseString();
            method = Instantiator::STRING;
         }

         loadScript(mode, script, method, entityName);
      }

      catch (string error) {
         s << filename << ": " << error << " (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      checkClosingTag("script");
   }

   /***************************************************************************/

   void Parser::parseEvent(string entityName, enum ParseMode mode) {

      stringstream s;

      string name = getAttribute("name");
      string function = parseString();

      try {
         setEvent(mode, name, function, entityName);
      }

      catch (string error) {
         s << filename << ": " << error << " (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      checkClosingTag("event");
   }

   /***************************************************************************/

   void Parser::parseObjects() {

      stringstream s;

      while (nextTag() && 2 == getDepth()) {

         if (0 == getTagName().compare("object")) {
            parseObject();
         }

         else if (instantiator->entityClassExists(getTagName(), entity::ENTITY_OBJECT)) {
            parseObject(getTagName());
         }

         else {
            s << filename << ": invalid tag <" << getTagName() << "> in "
               << "objects section (line "
               << xmlTextReaderGetParserLineNumber(reader) << ")";
            throw s.str();
         }
      }

      checkClosingTag("objects");
   }

   /***************************************************************************/

   void Parser::parseObject(string className) {

      stringstream s;

      if (!instantiator->entityExists(getAttribute("name"))) {
         s << filename << ": object '" << getAttribute("name") << "' was not "
            << "declared in the manifest (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      // Make sure entity is an object
      enum entity::EntityType type = instantiator->getEntityType(getAttribute("name"));

      if (entity::ENTITY_OBJECT != type) {
         s << filename << ": object type mismatch: '" << getAttribute("name")
            << "' is of type " << Entity::typeToStr(type) << ", but was declared in "
            << "<objects> (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      // Make sure object is the correct class
      string entityClass = instantiator->getEntityClass(getAttribute("name"));

      if (className != entityClass) {
         s << filename << ": object type mismatch: '" << getAttribute("name")
            << "' is of type " << entityClass << ", but was declared in "
            << "<objects> to be of type " << className << " (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      // set the object's default title
      s << "a " << getAttribute("name");
      entitySetter(getAttribute("name"), "title", s.str(), PARSE_ENTITY);

      parseObjectProperties(getAttribute("name"), PARSE_ENTITY, 3);
      checkClosingTag(className);
   }

   /***************************************************************************/

   void Parser::parseObjectProperties(string name, enum ParseMode mode, int depth) {

      stringstream s;
      static unordered_map<string, string> tagToProperty({
         {"title", "title"}, {"description", "longDesc"}, {"short", "shortDesc"},
         {"weight", "weight"}, {"takeable", "takeable"}, {"droppable", "droppable"},
         {"weapon", "weapon"}, {"damage", "damage"}
      });

      try {

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

            else if (0 == tag.compare("events")) {
               parseEvents(name, mode, depth + 1);
            }

            else if (tagToProperty.find(tag) != tagToProperty.end()) {
               string value = parseString();
               entitySetter(name, tagToProperty[tag], value, mode);
               checkClosingTag(tag);
            }

            else {
               s << filename << ": invalid tag <" << tag << "> in "
                  << "object or object class definition (line "
                  << xmlTextReaderGetParserLineNumber(reader) << ")";
               throw s.str();
            }
         }
      }

      catch (string error) {
         s << filename << ": " << error << " (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }
   }

   /***************************************************************************/

   void Parser::parsePlayer() {

      stringstream s;

      while (nextTag() && 2 == getDepth()) {

         if (0 == getTagName().compare("default")) {
            parseDefaultPlayer();
         }

         else {
            s << filename << ": invalid tag <" << getTagName() << "> in "
               << "player section (line "
               << xmlTextReaderGetParserLineNumber(reader) << ")";
            throw s.str();
         }
      }

      checkClosingTag("player");
   }

   /***************************************************************************/

   void Parser::parseDefaultPlayer() {

      stringstream s;
      static unordered_map<string, string> tagToProperty({
         {"alive", "alive"}, {"health", "health"}, {"maxhealth", "maxhealth"},
         {"attackable", "attackable"}, {"woundrate", "woundrate"},
         {"damagebarehands", "damagebarehands"}
      });

      try {

         while (nextTag() && 3 == getDepth()) {

            string tag = getTagName();

            if (0 == tag.compare("messages")) {
               parseMessages("", PARSE_DEFAULT_PLAYER, 4);
            }

            else if (0 == tag.compare("inventory")) {
               parseBeingInventory("", PARSE_DEFAULT_PLAYER, false);
            }

            else if (0 == tag.compare("respawn")) {
               Parser::parseBeingRespawn("", PARSE_DEFAULT_PLAYER, 4);
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
               s << filename << ": invalid tag <" << tag << "> in "
                  << "default player settings (line "
                  << xmlTextReaderGetParserLineNumber(reader) << ")";
               throw s.str();
            }
         }
      }

      catch (string error) {
         s << filename << ": " << error << " (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      checkClosingTag("default");
   }

   /***************************************************************************/

   void Parser::parseCreatures() {

      stringstream s;

      while (nextTag() && 2 == getDepth()) {

         if (0 == getTagName().compare("creature")) {
            parseCreature();
         }

         else if (instantiator->entityClassExists(getTagName(), entity::ENTITY_CREATURE)) {
            parseCreature(getTagName());
         }

         else {
            s << filename << ": invalid tag <" << getTagName() << "> in "
               << "creatures section (line "
               << xmlTextReaderGetParserLineNumber(reader) << ")";
            throw s.str();
         }
      }

      checkClosingTag("creatures");
   }

   /***************************************************************************/

   void Parser::parseCreature(string className) {

      stringstream s;

      if (!instantiator->entityExists(getAttribute("name"))) {
         s << filename << ": creature '" << getAttribute("name") << "' was not "
            << "declared in the manifest (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      // Make sure entity is a creature
      enum entity::EntityType type = instantiator->getEntityType(getAttribute("name"));

      if (entity::ENTITY_CREATURE != type) {
         s << filename << ": creature type mismatch: '" << getAttribute("name")
            << "' is of type " << Entity::typeToStr(type) << ", but was declared in "
            << "<creatures> (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      // Make sure creature is the correct class
      string entityClass = instantiator->getEntityClass(getAttribute("name"));

      if (className != entityClass) {
         s << filename << ": creature type mismatch: '" << getAttribute("name")
            << "' is of type " << entityClass << ", but was declared in "
            << "<creatures> to be of type " << className << " (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      // set the creature's default title and parse the rest of its properties
      entitySetter(getAttribute("name"), "title", getAttribute("name"), PARSE_ENTITY);
      parseCreatureProperties(getAttribute("name"), PARSE_ENTITY, 3);
      checkClosingTag(className);
   }

   /***************************************************************************/

   void Parser::parseCreatureProperties(string name, enum ParseMode mode, int depth) {

      stringstream s;
      bool counterAttackParsed = false;
      static unordered_map<string, string> tagToProperty({
         {"title", "title"}, {"description", "longDesc"}, {"short", "shortDesc"},
         {"alive", "alive"}, {"health", "health"}, {"maxhealth", "maxhealth"},
         {"attackable", "attackable"}, {"woundrate", "woundrate"},
         {"damagebarehands", "damagebarehands"}, {"counterattack", "counterattack"},
         {"allegiance", "allegiance"}
      });

      try {

         while (nextTag() && depth == getDepth()) {

            string tag = getTagName();

            if (0 == tag.compare("inventory")) {
               parseBeingInventory(name, mode, true);
            }

            else if (0 == tag.compare("respawn")) {
               Parser::parseBeingRespawn(name, mode, depth + 1);
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

            else if (0 == tag.compare("events")) {
               parseEvents(name, mode, depth + 1);
            }

            else if (tagToProperty.find(tag) != tagToProperty.end()) {
               string value = parseString();
               entitySetter(name, tagToProperty[tag], value, mode);
               checkClosingTag(tag);
            }

            else {
               s << filename << ": invalid tag <" << tag << "> in "
                  << "creature definition (line "
                  << xmlTextReaderGetParserLineNumber(reader) << ")";
               throw s.str();
            }
         }

         // Set default counter-attack rules
         if (!counterAttackParsed) {
            entitySetter(name, "counterattack.default", "", mode);
         }
      }

      catch (string error) {
         s << filename << ": " << error << " (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }
   }

   /***************************************************************************/

   void Parser::parseRooms() {

      stringstream s;

      while (nextTag() && 2 == getDepth()) {

         if (0 == getTagName().compare("room")) {
            parseRoom();
         }

         else if (instantiator->entityClassExists(getTagName(), entity::ENTITY_ROOM)) {
            parseRoom(getTagName());
         }

         else {
            s << filename << ": invalid tag <" << getTagName() << "> in "
               << "rooms section (line "
               << xmlTextReaderGetParserLineNumber(reader) << ")";
            throw s.str();
         }
      }

      checkClosingTag("rooms");
   }

   /***************************************************************************/

   void Parser::parseRoom(string className) {

      stringstream s;

      if (!instantiator->entityExists(getAttribute("name"))) {
         s << filename << ": room '" << getAttribute("name") << "' was not "
            << "declared in the manifest (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      // Make sure entity is a room
      enum entity::EntityType type = instantiator->getEntityType(getAttribute("name"));

      if (entity::ENTITY_ROOM != type) {
         s << filename << ": room type mismatch: '" << getAttribute("name")
            << "' is of type " << Entity::typeToStr(type) << ", but was declared in "
            << "<rooms> (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      // Make sure room is the correct class
      string entityClass = instantiator->getEntityClass(getAttribute("name"));

      if (className != entityClass) {
         s << filename << ": room type mismatch: '" << getAttribute("name")
            << "' is of type " << entityClass << ", but was declared in "
            << "<rooms> to be of type " << className << " (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      // set Room's default title and parse remaining properties
      entitySetter(getAttribute("name"), "title", getAttribute("name"), PARSE_ENTITY);
      parseRoomProperties(getAttribute("name"), PARSE_ENTITY, 3);
      checkClosingTag(className);
   }

   /***************************************************************************/

   void Parser::parseRoomProperties(string name, enum ParseMode mode, int depth) {

      stringstream s;
      static unordered_map<string, string> tagToProperty({
         {"title", "title"}, {"description", "longDesc"}, {"short", "shortDesc"}
      });

      try {

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

            else if (0 == tag.compare("events")) {
               parseEvents(name, mode, depth + 1);
            }

            else if (tagToProperty.find(tag) != tagToProperty.end()) {
               string value = parseString();
               entitySetter(name, tagToProperty[tag], value, mode);
               checkClosingTag(tag);
            }

            else {
               s << filename << ": invalid tag <" << tag << "> in "
                  << "room or class definition (line "
                  << xmlTextReaderGetParserLineNumber(reader) << ")";
               throw s.str();
            }
         }
      }

      catch (string error) {
         s << filename << ": " << error << " (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }
   }

   /***************************************************************************/

   void Parser::parseRoomConnection(string direction, string roomName,
   string connectTo, enum ParseMode mode) {

      stringstream s;

      try {
         entitySetter(roomName, "connection", direction + ":" + connectTo, mode);
      }

      catch (string error) {
         s << filename << ": " << error << " (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      checkClosingTag(direction);
   }

   /***************************************************************************/

   void Parser::parseRoomContains(string roomName, enum ParseMode mode) {

      stringstream s;

      while (nextTag() && 4 == getDepth()) {

         string tag = getTagName();

         if (0 == tag.compare("object") || 0 == tag.compare("creature")) {

            try {
               entitySetter(roomName, "contains", parseString(), mode);
            }

            catch (string error) {
               s << filename << ": " << error << " (line "
                  << xmlTextReaderGetParserLineNumber(reader) << ")";
               throw s.str();
            }
         }

         else {
            s << filename << ": invalid tag <" << getTagName() << "> in "
               << "rooms section (line "
               << xmlTextReaderGetParserLineNumber(reader) << ")";
            throw s.str();
         }

         checkClosingTag(tag);
      }

      checkClosingTag("contains");
   }

   /***************************************************************************/

   void Parser::parseCreatureAutoAttack(string creatureName, enum ParseMode mode,
   int depth) {

      stringstream s;
      static unordered_map<string, string> tagToProperty({
         {"enabled", "autoattack.enabled"}, {"repeat", "autoattack.repeat"},
         {"interval", "autoattack.interval"}
      });

      try {

         while (nextTag() && depth == getDepth()) {

            string tag = getTagName();

            if (tagToProperty.find(tag) != tagToProperty.end()) {
               string value = parseString();
               entitySetter(creatureName, tagToProperty[tag], value, mode);
               checkClosingTag(tag);
            }

            else {
               s << filename << ": invalid tag <" << getTagName() << "> in "
                  << "creature autoattack section (line "
                  << xmlTextReaderGetParserLineNumber(reader) << ")";
               throw s.str();
            }
         }
      }

      catch (string error) {
         s << filename << ": " << error << " (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      checkClosingTag("autoattack");
   }

   /***************************************************************************/

   void Parser::parseCreatureWandering(string creatureName, enum ParseMode mode) {

      stringstream s;
      static unordered_map<string, string> tagToProperty({
         {"enabled", "wandering.enabled"}, {"interval", "wandering.interval"},
         {"wanderlust", "wandering.wanderlust"}
      });

      try {

         while (nextTag() && 4 == getDepth()) {

            string tag = getTagName();

            if (tagToProperty.find(tag) != tagToProperty.end()) {
               string value = parseString();
               entitySetter(creatureName, tagToProperty[tag], value, mode);
               checkClosingTag(tag);
            }

            else {
               s << filename << ": invalid tag <" << tag << "> in "
                  << "creature wandering settings (line "
                  << xmlTextReaderGetParserLineNumber(reader) << ")";
               throw s.str();
            }
         }
      }

      catch (string error) {
         s << filename << ": " << error << " (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      checkClosingTag("wandering");
   }

   /***************************************************************************/

   void Parser::parseBeingRespawn(string beingName, enum ParseMode mode, int depth) {

      stringstream s;
      static unordered_map<string, string> tagToProperty({
         {"enabled", "respawn.enabled"}, {"interval", "respawn.interval"},
         {"lives", "respawn.lives"}
      });

      try {

         while (nextTag() && depth == getDepth()) {

            string tag = getTagName();

            if (tagToProperty.find(tag) != tagToProperty.end()) {
               string value = parseString();
               entitySetter(beingName, tagToProperty[tag], value, mode);
               checkClosingTag(tag);
            }

            else {
               s << filename << ": invalid tag <" << tag << "> in "
                  << "being respawn section (line "
                  << xmlTextReaderGetParserLineNumber(reader) << ")";
               throw s.str();
            }
         }
      }

      catch (string error) {
         s << filename << ": " << error << " (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      checkClosingTag("respawn");
   }

   /***************************************************************************/

   void Parser::parseBeingInventory(string beingName, enum ParseMode mode,
   bool allowObjects) {

      stringstream s;
      static unordered_map<string, string> tagToProperty({
         {"weight", "inventory.weight"}, {"object", "inventory.object"}
      });

      try {

         while (nextTag() && 4 == getDepth()) {

            string tag = getTagName();

            if (tagToProperty.find(tag) != tagToProperty.end()) {
               string value = parseString();
               entitySetter(beingName, tagToProperty[tag], value, mode);
               checkClosingTag(tag);
            }

            else {
               s << filename << ": invalid tag <" << tag << "> in "
                  << "inventory settings (line "
                  << xmlTextReaderGetParserLineNumber(reader) << ")";
               throw s.str();
            }
         }
      }

      catch (string error) {
         s << filename << ": " << error << " (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      checkClosingTag("inventory");
   }

   /***************************************************************************/

   void Parser::parseBeingAttributes(string beingName, enum ParseMode mode) {

      stringstream s;

      try {

         while (nextTag() && 4 == getDepth()) {

           string tag = getTagName();

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
               s << filename << ": invalid tag <" << tag << "> in "
                  << "default player's inventory settings (line "
                  << xmlTextReaderGetParserLineNumber(reader) << ")";
               throw s.str();
            }
         }
      }

      catch (string error) {
         s << filename << ": " << error << " (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      checkClosingTag("attributes");
   }

   /***************************************************************************/

   void Parser::parseThingAliases(string entityName, enum ParseMode mode, int depth) {

      stringstream s;

      try {

         while (nextTag() && depth == getDepth()) {

            if (0 == getTagName().compare("alias")) {
               string alias = parseString();
               entitySetter(entityName, "alias", alias, mode);
               checkClosingTag("alias");
            }

            else {
               s << filename << ": invalid tag <" << getTagName() << "> in "
                  << "aliases (line "
                  << xmlTextReaderGetParserLineNumber(reader) << ")";
               throw s.str();
            }
         }
      }

      catch (string error) {
         s << filename << ": " << error << " (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      checkClosingTag("aliases");
   }

   /***************************************************************************/

   string Parser::parseString() {

      stringstream s;
      string value = getNodeValue();
      value = trim(value);

      s << filename << ": Tag requires a value (line "
         << xmlTextReaderGetParserLineNumber(reader) << ")";

      if (value.length() > 0) {
         return value;
      }

      throw s.str();
   }

   /***************************************************************************/

   bool Parser::nextTag() {

      stringstream s;
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
         s << filename << ": expected opening tag (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      if (status < 0) {
         s << filename << ": unknown error reading " << filename << " (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
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

   string Parser::getAttribute(const char *name) {

      const char *attr;
      stringstream s;

      attr = (const char *)xmlTextReaderGetAttribute(reader, (xmlChar *)name);

      if (!attr) {
         s << filename << ": missing attribute '" << name << "' (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      return attr;
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

      // check to see if nextTag() encountered a closing tag
      if (lastClosedTag.length() > 0) {

         if (0 != lastClosedTag.compare(tag)) {
            s << "expected closing </" << tag << "> (line " <<
               xmlTextReaderGetParserLineNumber(reader) << ")";
            throw s.str();
         }

         lastClosedTag = "";
         return;
      }

      // TODO: skip over XML comments (right now, we'll get an error!)

      if (xmlTextReaderRead(reader) < 0) {
         s << "Unknown error reading " << filename << " (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      else if (XML_ELEMENT_DECL != xmlTextReaderNodeType(reader) ||
      0 != tag.compare(strToLower((const char *)xmlTextReaderConstName(reader)))) {
         s << "missing closing </" << tag << "> (line " <<
            xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }
   }
}}

