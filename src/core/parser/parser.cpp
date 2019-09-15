#include <memory>

#include "../include/parser/parser.h"
#include "../include/iostream/nullout.h"
#include "../include/iostream/nullin.h"
#include "../include/iostream/placeout.h"

// TODO: remove after testing
#include "../include/game.h"

using namespace std;

namespace trogdor { namespace core {


   // TODO: remove g as argument after testing
   Parser::Parser(std::unique_ptr<Instantiator> i, string gameFile, Game *g) {

      // TODO: remove after testing
      game = g;

      instantiator = std::move(i);

      filename = gameFile;

      reader = xmlReaderForFile(gameFile.c_str(), NULL, XML_PARSE_NOBLANKS);
      if (NULL == reader) {
         throw "failed to open " + gameFile + "!\n";
      }

      // TODO: remove after moving into instantiator
      gameL = make_shared<LuaState>();
      eventListener = new event::EventListener();
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
            parseEvents(gameL, eventListener, 2);
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

      while (nextTag() && 2 == getDepth()) {

         if (0 == getTagName().compare("enabled")) {
            string value = parseString();
            instantiator->gameSetter("introduction.enabled", value);
            checkClosingTag("enabled");
         }

         else if (0 == getTagName().compare("pause")) {
            string value = parseString();
            instantiator->gameSetter("introduction.pause", value);
            checkClosingTag("pause");
         }

         else if (0 == getTagName().compare("text")) {
            string value = parseString();
            instantiator->gameSetter("introduction.text", value);
            checkClosingTag("text");
         }

         else {
            s << filename << ": invalid tag <" << getTagName() <<
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
         string value = getNodeValue();
         value = trim(value);
         instantiator->gameSetter("meta", key + ":" + value);
         checkClosingTag(key);
      }

      checkClosingTag("meta");
   }

   /***************************************************************************/

   void Parser::parseEntityMeta(string entityName, enum ParseMode mode, int depth) {

      while (nextTag() && depth == getDepth()) {
         string key = getTagName();
         string value = getNodeValue();
         value = trim(value);
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
            parseSynonymVerb(getAttribute("action"));
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

   void Parser::parseSynonymVerb(string action) {

      game->setSynonym(parseString(), action);
      checkClosingTag("verb");
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

               string message = getNodeValue();
               message = trim(message);

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

   void Parser::parseEvents(const std::shared_ptr<LuaState> &L, EventListener *triggers, int depth) {

      stringstream s;

      while (nextTag() && depth == getDepth()) {

         if (0 == getTagName().compare("script")) {
            parseScript(L);
         }

         else if (0 == getTagName().compare("event")) {
            parseEvent(L, triggers);
         }


         else {
            s << filename << ": invalid tag <" << getTagName() << "> in "
               << "<events> (line " << xmlTextReaderGetParserLineNumber(reader)
               << ")";
            throw s.str();
         }
      }

      checkClosingTag("events");
   }

   /***************************************************************************/

   void Parser::parseScript(const std::shared_ptr<LuaState> &L) {

      // external script
/* TODO: stub for testing
      try {
         L->loadScriptFromFile(getAttribute("src"));
      }

      // inline script
      catch (string error) {
         L->loadScriptFromString(parseString());
      }
*/
      // TODO: delete this after testing
      try {
         cout << "Stub: Skipping script file: " << getAttribute("src") << endl;
      }

      catch (string error) {
         cout << "Stub: Skipping inline script: " << endl << parseString() << endl;
      }

      checkClosingTag("script");
   }

   /***************************************************************************/

   void Parser::parseEvent(const std::shared_ptr<LuaState> &L, EventListener *triggers) {

      string name = getAttribute("name");
      string function = parseString();
/* TODO: stub for testing
      LuaEventTrigger *trigger = new LuaEventTrigger(function, L);
      triggers->add(name.c_str(), trigger);
*/
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

      try {

         while (nextTag() && depth == getDepth()) {

            if (0 == getTagName().compare("title")) {
               string title = getNodeValue();
               title = trim(title);
               entitySetter(name, "title", title, mode);
               checkClosingTag("title");
            }

            else if (0 == getTagName().compare("description")) {
               string longdesc = getNodeValue();
               longdesc = trim(longdesc);
               entitySetter(name, "longDesc", longdesc, mode);
               checkClosingTag("description");
            }

            else if (0 == getTagName().compare("short")) {
               string shortdesc = getNodeValue();
               shortdesc = trim(shortdesc);
               entitySetter(name, "shortDesc", shortdesc, mode);
               checkClosingTag("short");
            }

            else if (0 == getTagName().compare("weight")) {
               string weight = getNodeValue();
               weight = trim(weight);
               entitySetter(name, "weight", weight, mode);
               checkClosingTag("weight");
            }

            else if (0 == getTagName().compare("takeable")) {
               string takeable = getNodeValue();
               takeable = trim(takeable);
               entitySetter(name, "takeable", takeable, mode);
               checkClosingTag("takeable");
            }

            else if (0 == getTagName().compare("droppable")) {
               string droppable = getNodeValue();
               droppable = trim(droppable);
               entitySetter(name, "droppable", droppable, mode);
               checkClosingTag("droppable");
            }

            else if (0 == getTagName().compare("weapon")) {
               string weapon = getNodeValue();
               weapon = trim(weapon);
               entitySetter(name, "weapon", weapon, mode);
               checkClosingTag("weapon");
            }

            else if (0 == getTagName().compare("damage")) {
               string damage = getNodeValue();
               damage = trim(damage);
               entitySetter(name, "damage", damage, mode);
               checkClosingTag("damage");
            }

            else if (0 == getTagName().compare("meta")) {
               parseEntityMeta(name, mode, depth + 1);
            }

            else if (0 == getTagName().compare("messages")) {
               parseMessages(name, mode, depth + 1);
            }

            else if (0 == getTagName().compare("aliases")) {
               parseThingAliases(name, mode, depth + 1);
            }

            else if (0 == getTagName().compare("events")) {
               // TODO: stub for testing
               parseEvents(nullptr, nullptr, depth + 1);
               //parseEvents(object->L, object->triggers, depth + 1);
            }

            else {
               s << filename << ": invalid tag <" << getTagName() << "> in "
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

      try {

         while (nextTag() && 3 == getDepth()) {

            if (0 == getTagName().compare("messages")) {
               parseMessages("", PARSE_DEFAULT_PLAYER, 4);
            }

            else if (0 == getTagName().compare("inventory")) {
               parseBeingInventory("", PARSE_DEFAULT_PLAYER, false);
            }

            else if (0 == getTagName().compare("respawn")) {
               Parser::parseBeingRespawn("", PARSE_DEFAULT_PLAYER, 4);
            }

            else if (0 == getTagName().compare("attributes")) {
               parseBeingAttributes("", PARSE_DEFAULT_PLAYER);
            }

            else if (0 == getTagName().compare("alive")) {
               string alive = getNodeValue();
               alive = trim(alive);
               entitySetter("", "alive", alive, PARSE_DEFAULT_PLAYER);
               checkClosingTag("alive");
            }

            else if (0 == getTagName().compare("health")) {
               string health = getNodeValue();
               health = trim(health);
               entitySetter("", "health", health, PARSE_DEFAULT_PLAYER);
               checkClosingTag("health");
            }

            else if (0 == getTagName().compare("maxhealth")) {
               string maxhealth = getNodeValue();
               maxhealth = trim(maxhealth);
               entitySetter("", "maxhealth", maxhealth, PARSE_DEFAULT_PLAYER);
               checkClosingTag("maxhealth");
            }

            else if (0 == getTagName().compare("attackable")) {
               string attackable = getNodeValue();
               attackable = trim(attackable);
               entitySetter("", "attackable", attackable, PARSE_DEFAULT_PLAYER);
               checkClosingTag("attackable");
            }

            else if (0 == getTagName().compare("woundrate")) {
               string rate = getNodeValue();
               rate = trim(rate);
               entitySetter("", "woundrate", rate, PARSE_DEFAULT_PLAYER);
               checkClosingTag("woundrate");
            }

            else if (0 == getTagName().compare("damagebarehands")) {
               string damage = getNodeValue();
               damage = trim(damage);
               entitySetter("", "damagebarehands", damage, PARSE_DEFAULT_PLAYER);
               checkClosingTag("damagebarehands");
            }

            else {
               s << filename << ": invalid tag <" << getTagName() << "> in "
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

      try {

         while (nextTag() && depth == getDepth()) {

            if (0 == getTagName().compare("title")) {
               string title = getNodeValue();
               title = trim(title);
               entitySetter(name, "title", title, mode);
               checkClosingTag("title");
            }

            else if (0 == getTagName().compare("description")) {
               string longdesc = getNodeValue();
               longdesc = trim(longdesc);
               entitySetter(name, "longDesc", longdesc, mode);
               checkClosingTag("description");
            }

            else if (0 == getTagName().compare("short")) {
               string shortdesc = getNodeValue();
               shortdesc = trim(shortdesc);
               entitySetter(name, "shortDesc", shortdesc, mode);
               checkClosingTag("short");
            }

            else if (0 == getTagName().compare("alive")) {
               string alive = getNodeValue();
               alive = trim(alive);
               entitySetter(name, "alive", alive, mode);
               checkClosingTag("alive");
            }

            else if (0 == getTagName().compare("health")) {
               string health = getNodeValue();
               health = trim(health);
               entitySetter(name, "health", health, mode);
               checkClosingTag("health");
            }

            else if (0 == getTagName().compare("maxhealth")) {
               string maxhealth = getNodeValue();
               maxhealth = trim(maxhealth);
               entitySetter(name, "maxhealth", maxhealth, mode);
               checkClosingTag("maxhealth");
            }

            else if (0 == getTagName().compare("attackable")) {
               string attackable = getNodeValue();
               attackable = trim(attackable);
               entitySetter(name, "attackable", attackable, mode);
               checkClosingTag("attackable");
            }

            else if (0 == getTagName().compare("woundrate")) {
               string rate = getNodeValue();
               rate = trim(rate);
               entitySetter(name, "woundrate", rate, mode);
               checkClosingTag("woundrate");
            }

            else if (0 == getTagName().compare("damagebarehands")) {
               string damage = getNodeValue();
               damage = trim(damage);
               entitySetter(name, "damagebarehands", damage, mode);
               checkClosingTag("damagebarehands");
            }

            else if (0 == getTagName().compare("counterattack")) {
               string counterattack = getNodeValue();
               counterattack = trim(counterattack);
               entitySetter(name, "counterattack", counterattack, mode);
               counterAttackParsed = true;
               checkClosingTag("counterattack");
            }

            else if (0 == getTagName().compare("allegiance")) {
               string allegiance = getNodeValue();
               allegiance = trim(allegiance);
               entitySetter(name, "allegiance", allegiance, mode);
               checkClosingTag("allegiance");
            }

            else if (0 == getTagName().compare("inventory")) {
               parseBeingInventory(name, mode, true);
            }

            else if (0 == getTagName().compare("respawn")) {
               Parser::parseBeingRespawn(name, mode, depth + 1);
            }

            else if (0 == getTagName().compare("autoattack")) {
               parseCreatureAutoAttack(name, mode, depth + 1);
            }

            else if (0 == getTagName().compare("wandering")) {
               parseCreatureWandering(name, mode);
            }

            else if (0 == getTagName().compare("attributes")) {
               parseBeingAttributes(name, mode);
            }

            else if (0 == getTagName().compare("meta")) {
               parseEntityMeta(name, mode, depth + 1);
            }

            else if (0 == getTagName().compare("messages")) {
               parseMessages(name, mode, depth + 1);
            }

            else if (0 == getTagName().compare("aliases")) {
               parseThingAliases(name, mode, depth + 1);
            }

            else if (0 == getTagName().compare("events")) {
               // TODO: stub for now to test without event parsing
               parseEvents(nullptr, nullptr, depth + 1);
               //parseEvents(creature->L, creature->triggers, depth + 1);
            }

            else {
               s << filename << ": invalid tag <" << getTagName() << "> in "
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

      try {

         while (nextTag() && depth == getDepth()) {

            if (0 == getTagName().compare("title")) {
               string title = getNodeValue();
               title = trim(title);
               entitySetter(name, "title", title, mode);
               checkClosingTag("title");
            }

            else if (0 == getTagName().compare("description")) {
               string longdesc = getNodeValue();
               longdesc = trim(longdesc);
               entitySetter(name, "longDesc", longdesc, mode);
               checkClosingTag("description");
            }

            else if (0 == getTagName().compare("short")) {
               string shortdesc = getNodeValue();
               shortdesc = trim(shortdesc);
               entitySetter(name, "shortDesc", shortdesc, mode);
               checkClosingTag("short");
            }

            else if (isDirection(getTagName())) {
               string direction = getTagName();
               string connection = parseString();
               parseRoomConnection(direction, name, connection, mode);
            }

            else if (0 == getTagName().compare("contains")) {
               parseRoomContains(name, mode);
            }

            else if (0 == getTagName().compare("meta")) {
               parseEntityMeta(name, mode, depth + 1);
            }

            else if (0 == getTagName().compare("messages")) {
               parseMessages(name, mode, depth + 1);
            }

            else if (0 == getTagName().compare("events")) {
               // TODO: stub for testing without actually parsing events
               parseEvents(nullptr, nullptr, depth + 1);
               //parseEvents(room->L, room->triggers, depth + 1);
            }

            else {
               s << filename << ": invalid tag <" << getTagName() << "> in "
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

      try {

         while (nextTag() && depth == getDepth()) {

            if (0 == getTagName().compare("enabled")) {
               string enabled = getNodeValue();
               enabled = trim(enabled);
               entitySetter(creatureName, "autoattack.enabled", enabled, mode);
               checkClosingTag("enabled");
            }

            else if (0 == getTagName().compare("repeat")) {
               string repeat = getNodeValue();
               repeat = trim(repeat);
               entitySetter(creatureName, "autoattack.repeat", repeat, mode);
               checkClosingTag("repeat");
            }

            else if (0 == getTagName().compare("interval")) {
               string interval = getNodeValue();
               interval = trim(interval);
               entitySetter(creatureName, "autoattack.interval", interval, mode);
               checkClosingTag("interval");
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

      try {

         while (nextTag() && 4 == getDepth()) {

            string tag = getTagName();

            if (0 == tag.compare("enabled")) {
               string enabled = getNodeValue();
               enabled = trim(enabled);
               entitySetter(creatureName, "wandering.enabled", enabled, mode);
               checkClosingTag("enabled");
            }

            else if (0 == tag.compare("interval")) {
               string interval = getNodeValue();
               interval = trim(interval);
               entitySetter(creatureName, "wandering.interval", interval, mode);
               checkClosingTag("interval");
            }

            else if (0 == tag.compare("wanderlust")) {
               string wanderlust = getNodeValue();
               wanderlust = trim(wanderlust);
               entitySetter(creatureName, "wandering.wanderlust", wanderlust, mode);
               checkClosingTag("wanderlust");
            }

            else {
               s << filename << ": invalid tag <" << getTagName() << "> in "
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

      try {

         while (nextTag() && depth == getDepth()) {

            if (0 == getTagName().compare("enabled")) {
               string respawnEnabled = getNodeValue();
               respawnEnabled = trim(respawnEnabled);
               entitySetter(beingName, "respawn.enabled", respawnEnabled, mode);
               checkClosingTag("enabled");
            }

            else if (0 == getTagName().compare("interval")) {
               string interval = getNodeValue();
               interval = trim(interval);
               entitySetter(beingName, "respawn.interval", interval, mode);
               checkClosingTag("interval");
            }

            else if (0 == getTagName().compare("lives")) {
               string lives = getNodeValue();
               lives = trim(lives);
               entitySetter(beingName, "respawn.lives", lives, mode);
               checkClosingTag("lives");
            }

            else {
               s << filename << ": invalid tag <" << getTagName() << "> in "
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

      try {

         while (nextTag() && 4 == getDepth()) {

            if (0 == getTagName().compare("weight")) {
               string weight = getNodeValue();
               weight = trim(weight);
               entitySetter(beingName, "inventory.weight", weight, mode);
               checkClosingTag("weight");
            }

            else if (true == allowObjects && 0 == getTagName().compare("object")) {
               string objectName = getNodeValue();
               objectName = trim(objectName);
               entitySetter(beingName, "inventory.object", objectName, mode);
               checkClosingTag("object");
            }

            else {
               s << filename << ": invalid tag <" << getTagName() << "> in "
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
               string value = getNodeValue();
               value = trim(value);
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
               string alias = getNodeValue();
               alias = trim(alias);
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

