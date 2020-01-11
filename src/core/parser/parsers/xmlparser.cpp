#include <memory>

#include <trogdor/utility.h>

#include <trogdor/iostream/nullout.h>
#include <trogdor/iostream/nullin.h>
#include <trogdor/iostream/placeout.h>

#include <trogdor/parser/parsers/xmlparser.h>
#include <trogdor/exception/validationexception.h>
#include <trogdor/exception/parseexception.h>

namespace trogdor {


   XMLParser::~XMLParser() {

      if (reader) {
         xmlFreeTextReader(reader);
      }
   }

   /***************************************************************************/

   void XMLParser::parse(std::string filename) {

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

      instantiator->instantiate(ast);
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

         if (0 == getTagName().compare("vocabulary")) {
              parseVocabulary();
         }

         else if (0 == getTagName().compare("events")) {
              parseEvents("", "game", 2);
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
            throw ParseException(std::string("invalid section <") + getTagName() + ">");
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
            throw ParseException(std::string("invalid tag <") + getTagName() + "> in <classes>");
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
            throw ParseException(std::string("invalid tag <") + getTagName()
               + "> in rooms section of <classes>");
         }
      }

      checkClosingTag("rooms");
   }

   /***************************************************************************/

   void XMLParser::parseClassesRoom() {

      std::string name = getAttribute("class");

      if (isClassNameReserved(name)) {
         throw ParseException(std::string("class name '") + name + "' is reserved");
      }

      declaredEntityClasses[name] = entity::ENTITY_ROOM;

      ast->appendChild(ASTDefineEntityClass(
         name,
         entity::Entity::typeToStr(entity::ENTITY_ROOM),
         xmlTextReaderGetParserLineNumber(reader)
      ));

      parseRoomProperties(name, "class", 4);
      checkClosingTag("room");
   }

   /***************************************************************************/

   void XMLParser::parseClassesObjects() {

      while (nextTag() && 3 == getDepth()) {

         if (0 == getTagName().compare("object")) {
            parseClassesObject();
         }

         else {
            throw ParseException(std::string("invalid tag <") + getTagName()
               + "> in objects section of <classes>");
         }
      }

      checkClosingTag("objects");
   }

   /***************************************************************************/

   void XMLParser::parseClassesObject() {

      std::string name = getAttribute("class");

      if (isClassNameReserved(name)) {
         throw ParseException(std::string("class name '") + name + "' is reserved");
      }

      declaredEntityClasses[name] = entity::ENTITY_OBJECT;

      ast->appendChild(ASTDefineEntityClass(
         name,
         entity::Entity::typeToStr(entity::ENTITY_OBJECT),
         xmlTextReaderGetParserLineNumber(reader)
      ));

      // Don't pass object! It's been moved and is no longer a valid pointer.
      parseObjectProperties(name, "class", 4);
      checkClosingTag("object");
   }

   /***************************************************************************/

   void XMLParser::parseClassesCreatures() {

      while (nextTag() && 3 == getDepth()) {

         if (0 == getTagName().compare("creature")) {
            parseClassesCreature();
         }

         else {
            throw ParseException(std::string("invalid tag <") + getTagName()
               + "> in creatures section of <classes>");
         }
      }

      checkClosingTag("creatures");
   }

   /***************************************************************************/

   void XMLParser::parseClassesCreature() {

      std::string name = getAttribute("class");

      if (isClassNameReserved(name)) {
         throw ParseException(std::string("class name '") + name + "' is reserved");
      }

      declaredEntityClasses[name] = entity::ENTITY_CREATURE;

      ast->appendChild(ASTDefineEntityClass(
         name,
         entity::Entity::typeToStr(entity::ENTITY_CREATURE),
         xmlTextReaderGetParserLineNumber(reader)
      ));

      // Don't pass creature! It's been moved and is no longer a valid pointer.
      parseCreatureProperties(name, "class", 4);
      checkClosingTag("creature");
   }

   /***************************************************************************/

   void XMLParser::parseIntroduction() {

      static std::unordered_map<std::string, std::string> tagToProperty({
         {"enabled", "introduction.enabled"}, {"pause", "introduction.pause"},
         {"text", "introduction.text"}
      });

      while (nextTag() && 2 == getDepth()) {

         std::string tag = getTagName();

         if (tagToProperty.find(tag) != tagToProperty.end()) {

            std::string value = parseString();

            ast->appendChild(ASTSetProperty(
               "game",
               tagToProperty[tag],
               value,
               xmlTextReaderGetParserLineNumber(reader)
            ));

            checkClosingTag(tag);
         }

         else {
            throw ParseException(std::string("invalid tag <") + tag
               + "> in <introduction>");
         }
      }

      checkClosingTag("introduction");
   }

   /***************************************************************************/

   void XMLParser::parseGameMeta() {

      while (nextTag() && 2 == getDepth()) {

         std::string key = getTagName();
         std::string value = parseString();

         ast->appendChild(ASTSetMeta(
            "game",
            key,
            value,
            xmlTextReaderGetParserLineNumber(reader)
         ));

         checkClosingTag(key);
      }

      checkClosingTag("meta");
   }

   /***************************************************************************/

   void XMLParser::parseEntityMeta(std::string entityName, std::string targetType, int depth) {

      while (nextTag() && depth == getDepth()) {

         std::string key = getTagName();
         std::string value = parseString();

         ast->appendChild(ASTSetMeta(
            targetType,
            key,
            value,
            xmlTextReaderGetParserLineNumber(reader),
            entityName
         ));

         checkClosingTag(key);
      }

      checkClosingTag("meta");
   }

   /***************************************************************************/

   void XMLParser::parseVocabulary() {

      // parse the remaining sections
      while (nextTag() && 2 == getDepth()) {

         if (0 == getTagName().compare("directions")) {
            parseDirections();
         }

         else if (0 == getTagName().compare("synonyms")) {
            parseSynonyms();
         }

         else {
            throw ParseException(std::string("invalid section <") + getTagName() + ">");
         }
      }

      checkClosingTag("vocabulary");
   }

   /***************************************************************************/

   void XMLParser::parseDirections() {

      while (nextTag() && 3 == getDepth()) {

         if (0 == getTagName().compare("direction")) {

            std::string direction = parseString();

            ast->appendChild(ASTDefineDirection(
               direction,
               xmlTextReaderGetParserLineNumber(reader)
            ));

            customDirections.insert(direction);
            checkClosingTag("direction");
         }

         else {
            throw ParseException(std::string("invalid tag <") + getTagName()
               + "> in <directions>");
         }
      }

      checkClosingTag("directions");
   }

   /***************************************************************************/

   void XMLParser::parseSynonyms() {

      while (nextTag() && 3 == getDepth()) {

         if (0 == getTagName().compare("verb")) {

            std::string action = getAttribute("verb");
            action = trim(action);
            std::string synonym = parseString();
            synonym = trim(synonym);

            ast->appendChild(ASTDefineVerbSynonym(
               action,
               synonym,
               xmlTextReaderGetParserLineNumber(reader)
            ));

            checkClosingTag("verb");
         }

         else if (0 == getTagName().compare("direction")) {

            std::string direction = getAttribute("direction");
            direction = trim(direction);
            std::string synonym = parseString();
            synonym = trim(synonym);

            ast->appendChild(ASTDefineDirectionSynonym(
               direction,
               synonym,
               xmlTextReaderGetParserLineNumber(reader)
            ));

            customDirections.insert(synonym);
            checkClosingTag("direction");
         }

         else {
            throw ParseException(std::string("invalid tag <") + getTagName()
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
            throw ParseException(std::string("invalid tag <") + getTagName()
               + "> in <manifest>");
         }
      }

      checkClosingTag("manifest");
   }

   /***************************************************************************/

   void XMLParser::parseManifestRooms() {

      while (nextTag() && 3 == getDepth()) {

         if (0 == getTagName().compare("room")) {
            parseManifestRoom();
         }

         else if (entityClassDeclared(getTagName(), entity::ENTITY_ROOM)) {
            parseManifestRoom(getTagName());
         }

         else {
            throw ParseException(std::string("invalid tag <") + getTagName()
               + "> in rooms section of <manifest>");
         }
      }

      checkClosingTag("rooms");
   }

   /***************************************************************************/

   void XMLParser::parseManifestRoom(std::string className) {

      std::string name = getAttribute("name");

      declaredEntities[name] = {
         name,
         className,
         entity::ENTITY_ROOM
      };

      ast->appendChild(ASTDefineEntity(
         name,
         entity::Entity::typeToStr(entity::ENTITY_ROOM),
         className,
         xmlTextReaderGetParserLineNumber(reader)
      ));

      checkClosingTag(className);
   }

   /***************************************************************************/

   void XMLParser::parseManifestObjects() {

      while (nextTag() && 3 == getDepth()) {

         if (0 == getTagName().compare("object")) {
            parseManifestObject();
         }

         else if (entityClassDeclared(getTagName(), entity::ENTITY_OBJECT)) {
            parseManifestObject(getTagName());
         }

         else {
            throw ParseException(std::string("<") + getTagName()
               + "> is not an object class in objects section of <manifest>");
         }
      }

      checkClosingTag("objects");
   }

   /***************************************************************************/

   void XMLParser::parseManifestObject(std::string className) {

      std::string name = getAttribute("name");

      declaredEntities[name] = {
         name,
         className,
         entity::ENTITY_OBJECT
      };

      ast->appendChild(ASTDefineEntity(
         name,
         entity::Entity::typeToStr(entity::ENTITY_OBJECT),
         className,
         xmlTextReaderGetParserLineNumber(reader)
      ));

      checkClosingTag(className);
   }

   /***************************************************************************/

   void XMLParser::parseManifestCreatures() {

      while (nextTag() && 3 == getDepth()) {

         if (0 == getTagName().compare("creature")) {
            parseManifestCreature();
         }

         else if (entityClassDeclared(getTagName(), entity::ENTITY_CREATURE)) {
            parseManifestCreature(getTagName());
         }

         else {
            throw ParseException(std::string("invalid tag <") + getTagName()
               + "> in creatures section of <manifest>");
         }
      }

      checkClosingTag("creatures");
   }

   /***************************************************************************/

   void XMLParser::parseManifestCreature(std::string className) {

      std::string name = getAttribute("name");

      declaredEntities[name] = {
         name,
         className,
         entity::ENTITY_CREATURE
      };

      ast->appendChild(ASTDefineEntity(
         name,
         entity::Entity::typeToStr(entity::ENTITY_CREATURE),
         className,
         xmlTextReaderGetParserLineNumber(reader)
      ));

      checkClosingTag(className);
   }

   /***************************************************************************/

   void XMLParser::parseMessages(std::string entityName, std::string targetType, int depth) {

      while (nextTag() && depth == getDepth()) {

         if (0 == getTagName().compare("message")) {

            std::string messageName = getAttribute("name");
            messageName = trim(messageName);
            std::string message = parseString();

            ast->appendChild(ASTSetMessage(
               targetType,
               messageName,
               message,
               xmlTextReaderGetParserLineNumber(reader),
               entityName
            ));

            checkClosingTag("message");
         }

         else {
            throw ParseException("expected <message> in <messages>");
         }
      }

      checkClosingTag("messages");
   }

   /***************************************************************************/

   void XMLParser::parseEntityTags(std::string entityName, std::string targetType, int depth) {

      while (nextTag() && depth == getDepth()) {

         if (0 == getTagName().compare("tag")) {

            std::string tag = parseString();

            ast->appendChild(ASTSetTag(
               targetType,
               tag,
               xmlTextReaderGetParserLineNumber(reader),
               entityName
            ));

            checkClosingTag("tag");
         }

         else if (0 == getTagName().compare("remove")) {

            std::string tag = parseString();

            ast->appendChild(ASTRemoveTag(
               targetType,
               tag,
               xmlTextReaderGetParserLineNumber(reader),
               entityName
            ));

            checkClosingTag("remove");
         }

         else {
            throw ParseException("expected <tag> or <remove> in <tags>");
         }
      }

      checkClosingTag("tags");
   }

   /***************************************************************************/

   void XMLParser::parseEvents(std::string entityName, std::string targetType, int depth) {

      while (nextTag() && depth == getDepth()) {

         if (0 == getTagName().compare("script")) {
            parseScript(entityName, targetType);
         }

         else if (0 == getTagName().compare("event")) {
            parseEvent(entityName, targetType);
         }

         else {
            throw ParseException(std::string("invalid tag <") + getTagName()
               + "> in <events>");
         }
      }

      checkClosingTag("events");
   }

   /***************************************************************************/

   void XMLParser::parseScript(std::string entityName, std::string targetType) {

      std::string script;
      std::string scriptMode;

      try {
         script = getAttribute("src");
         scriptMode = "file";
      }

      // There's no src attribute, so we're either parsing an inline script or
      // we're about to encounter an error ;)
      catch (const ParseException &e) {
         script = parseString();
         scriptMode = "string";
      }

      ast->appendChild(ASTLoadScript(
         targetType,
         scriptMode,
         script,
         xmlTextReaderGetParserLineNumber(reader),
         entityName
      ));

      checkClosingTag("script");
   }

   /***************************************************************************/

   void XMLParser::parseEvent(std::string entityName, std::string targetType) {

      std::string name = getAttribute("name");
      std::string function = parseString();

      ast->appendChild(ASTSetEvent(
         targetType,
         name,
         function,
         xmlTextReaderGetParserLineNumber(reader),
         entityName
      ));

      checkClosingTag("event");
   }

   /***************************************************************************/

   void XMLParser::parseObjects() {

      while (nextTag() && 2 == getDepth()) {

         if (0 == getTagName().compare("object")) {
            parseObject();
         }

         else if (entityClassDeclared(getTagName(), entity::ENTITY_OBJECT)) {
            parseObject(getTagName());
         }

         else {
            throw ParseException(std::string("invalid tag <") + getTagName()
               + "> in <objects>");
         }
      }

      checkClosingTag("objects");
   }

   /***************************************************************************/

   void XMLParser::parseObject(std::string className) {

      std::string name = getAttribute("name");

      // Make sure entity exists
      if (!entityDeclared(name)) {
         throw ParseException(std::string("object '") + name
            + "' was not declared in <manifest>");
      }

      // Make sure entity is an object
      else if (entity::ENTITY_OBJECT != declaredEntities[name].type) {
         throw ParseException(std::string("object type mismatch: '")
            + name + "' is of type "
            + Entity::typeToStr(declaredEntities[name].type)
            + ", but was declared in <objects>");
      }

      // Make sure object is the correct class
      else if (className != declaredEntities[name].className) {
         throw ParseException(std::string("object type mismatch: '")
            + name + "' is of class "
            + declaredEntities[name].className
            + ", but was declared in <objects> to be of class " + className);
      }

      // set the object's default title
      ast->appendChild(ASTSetProperty(
         "entity",
         "title",
         std::string("a ") + name,
         xmlTextReaderGetParserLineNumber(reader),
         name
      ));

      parseObjectProperties(name, "entity", 3);
      checkClosingTag(className);
   }

   /***************************************************************************/

   void XMLParser::parseObjectProperties(std::string name, std::string targetType, int depth) {

      static std::unordered_map<std::string, std::string> tagToProperty({
         {"title", "title"}, {"description", "longDesc"}, {"short", "shortDesc"},
         {"weight", "weight"}, {"damage", "damage"}
      });

      while (nextTag() && depth == getDepth()) {

         std::string tag = getTagName();

         if (0 == tag.compare("meta")) {
            parseEntityMeta(name, targetType, depth + 1);
         }

         else if (0 == tag.compare("messages")) {
            parseMessages(name, targetType, depth + 1);
         }

         else if (0 == tag.compare("aliases")) {
            parseThingAliases(name, targetType, depth + 1);
         }

         else if (0 == tag.compare("tags")) {
            parseEntityTags(name, targetType, depth + 1);
         }

         else if (0 == tag.compare("events")) {
            parseEvents(name, targetType, depth + 1);
         }

         else if (tagToProperty.find(tag) != tagToProperty.end()) {

            std::string value = parseString();

            ast->appendChild(ASTSetProperty(
               targetType,
               tagToProperty[tag],
               value,
               xmlTextReaderGetParserLineNumber(reader),
               name
            ));

            checkClosingTag(tag);
         }

         else {
            throw ParseException(std::string("invalid tag <") + tag
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
            throw ParseException(std::string("invalid tag <") + getTagName()
               + "> in <player>");
         }
      }

      checkClosingTag("player");
   }

   /***************************************************************************/

   void XMLParser::parseDefaultPlayer() {

      static std::unordered_map<std::string, std::string> tagToProperty({
         {"alive", "alive"}, {"health", "health"}, {"maxhealth", "maxhealth"},
         {"woundrate", "woundrate"}, {"damagebarehands", "damagebarehands"}
      });

      while (nextTag() && 3 == getDepth()) {

         std::string tag = getTagName();

         if (0 == tag.compare("messages")) {
            parseMessages("", "defaultPlayer", 4);
         }

         else if (0 == tag.compare("tags")) {
            parseEntityTags("", "defaultPlayer", 4);
         }

         else if (0 == tag.compare("inventory")) {
            parseBeingInventory("", "defaultPlayer", false);
         }

         else if (0 == tag.compare("respawn")) {
            parseBeingRespawn("", "defaultPlayer", 4);
         }

         else if (0 == tag.compare("attributes")) {
            parseBeingAttributes("", "defaultPlayer");
         }

         else if (tagToProperty.find(tag) != tagToProperty.end()) {

            std::string value = parseString();

            ast->appendChild(ASTSetProperty(
               "defaultPlayer",
               tagToProperty[tag],
               value,
               xmlTextReaderGetParserLineNumber(reader)
            ));

            checkClosingTag(tag);
         }

         else {
            throw ParseException(std::string("invalid tag <") + tag
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

         else if (entityClassDeclared(getTagName(), entity::ENTITY_CREATURE)) {
            parseCreature(getTagName());
         }

         else {
            throw ParseException(std::string("invalid tag <") + getTagName()
               + "> in <creatures>");
         }
      }

      checkClosingTag("creatures");
   }

   /***************************************************************************/

   void XMLParser::parseCreature(std::string className) {

      std::string name = getAttribute("name");

      if (!entityDeclared(name)) {
         throw ParseException(std::string("creature '") + name
            + "' was not declared in <manifest>");
      }

      // Make sure entity is a creature
      else if (entity::ENTITY_CREATURE != declaredEntities[name].type) {
         throw ParseException(std::string("creature type mismatch: '")
            + name + "' is of type " + Entity::typeToStr(declaredEntities[name].type)
            + ", but was declared in <creatures>");
      }

      // Make sure creature is the correct class
      else if (className != declaredEntities[name].className) {
         throw ParseException(std::string("creature type mismatch: '")
            + name + "' is of class " + declaredEntities[name].className
            + ", but was declared in <creatures> to be of class " + className);
      }

      // set the creature's default title and parse the rest of its properties
      ast->appendChild(ASTSetProperty(
         "entity",
         "title",
         name,
         xmlTextReaderGetParserLineNumber(reader),
         name
      ));

      parseCreatureProperties(name, "entity", 3);
      checkClosingTag(className);
   }

   /***************************************************************************/

   void XMLParser::parseCreatureProperties(std::string name, std::string targetType, int depth) {

      bool counterAttackParsed = false;

      static std::unordered_map<std::string, std::string> tagToProperty({
         {"title", "title"}, {"description", "longDesc"}, {"short", "shortDesc"},
         {"alive", "alive"}, {"health", "health"}, {"maxhealth", "maxhealth"},
         {"woundrate", "woundrate"}, {"damagebarehands", "damagebarehands"},
         {"counterattack", "counterattack"}, {"allegiance", "allegiance"}
      });

      while (nextTag() && depth == getDepth()) {

         std::string tag = getTagName();

         if (0 == tag.compare("inventory")) {
            parseBeingInventory(name, targetType, true);
         }

         else if (0 == tag.compare("respawn")) {
            parseBeingRespawn(name, targetType, depth + 1);
         }

         else if (0 == tag.compare("autoattack")) {
            parseCreatureAutoAttack(name, targetType, depth + 1);
         }

         else if (0 == tag.compare("wandering")) {
            parseCreatureWandering(name, targetType);
         }

         else if (0 == tag.compare("attributes")) {
            parseBeingAttributes(name, targetType);
         }

         else if (0 == tag.compare("meta")) {
            parseEntityMeta(name, targetType, depth + 1);
         }

         else if (0 == tag.compare("messages")) {
            parseMessages(name, targetType, depth + 1);
         }

         else if (0 == tag.compare("aliases")) {
            parseThingAliases(name, targetType, depth + 1);
         }

         else if (0 == tag.compare("tags")) {
            parseEntityTags(name, targetType, depth + 1);
         }

         else if (0 == tag.compare("events")) {
            parseEvents(name, targetType, depth + 1);
         }

         else if (tagToProperty.find(tag) != tagToProperty.end()) {

            std::string value = parseString();

            ast->appendChild(ASTSetProperty(
               targetType,
               tagToProperty[tag],
               value,
               xmlTextReaderGetParserLineNumber(reader),
               name
            ));

            checkClosingTag(tag);
         }

         else {
            throw ParseException(std::string("invalid tag <") + tag
               + "> in creature definition");
         }
      }

      // Set default counter-attack rules
      // TODO: I really don't like this property. It'll stay for now, but I should
      // eventually change this with some sort of observer thing where, when
      // the allegiance property is changed, it triggers a re-evaluation of the
      // counterattack (only if counterattack was never set explicitly. Will need
      // to think about the architecture of this...)
      if (!counterAttackParsed) {

         ast->appendChild(ASTSetProperty(
            targetType,
            "counterattack.default",
            "",
            xmlTextReaderGetParserLineNumber(reader),
            name
         ));
      }
   }

   /***************************************************************************/

   void XMLParser::parseRooms() {

      while (nextTag() && 2 == getDepth()) {

         if (0 == getTagName().compare("room")) {
            parseRoom();
         }

         else if (entityClassDeclared(getTagName(), entity::ENTITY_ROOM)) {
            parseRoom(getTagName());
         }

         else {
            throw ParseException(std::string("invalid tag <") + getTagName()
               + "> in <rooms>");
         }
      }

      checkClosingTag("rooms");
   }

   /***************************************************************************/

   void XMLParser::parseRoom(std::string className) {

      std::string name = getAttribute("name");

      if (!entityDeclared(name)) {
         throw ParseException(std::string("room '") + name
            + "' was not declared in <manifest>");
      }

      // Make sure entity is a room
      else if (entity::ENTITY_ROOM != declaredEntities[name].type) {
         throw ParseException(std::string("room type mismatch: '")
            + name + "' is of type "
            + Entity::typeToStr(declaredEntities[name].type)
            + ", but was declared in <rooms>");
      }

      // Make sure room is the correct class
      else if (className != declaredEntities[name].className) {
         throw ParseException(std::string("room type mismatch: '")
            + name + "' is of class "
            + declaredEntities[name].className
            + ", but was declared in <rooms> to be of type " + className);
      }

      // set Room's default title and parse remaining properties
      ast->appendChild(ASTSetProperty(
         "entity",
         "title",
         name,
         xmlTextReaderGetParserLineNumber(reader),
         name
      ));

      parseRoomProperties(name, "entity", 3);
      checkClosingTag(className);
   }

   /***************************************************************************/

   void XMLParser::parseRoomProperties(std::string name, std::string targetType, int depth) {

      static std::unordered_map<std::string, std::string> tagToProperty({
         {"title", "title"}, {"description", "longDesc"}, {"short", "shortDesc"}
      });

      while (nextTag() && depth == getDepth()) {

         std::string tag = getTagName();

         if (
            vocabulary.isDirection(tag) ||
            customDirections.end() != customDirections.find(tag)
         ) {
            std::string connection = parseString();
            parseRoomConnection(tag, name, connection, targetType);
         }

         else if (0 == tag.compare("contains")) {
            parseRoomContains(name, targetType);
         }

         else if (0 == tag.compare("meta")) {
            parseEntityMeta(name, targetType, depth + 1);
         }

         else if (0 == tag.compare("messages")) {
            parseMessages(name, targetType, depth + 1);
         }

         else if (0 == tag.compare("tags")) {
            parseEntityTags(name, targetType, depth + 1);
         }

         else if (0 == tag.compare("events")) {
            parseEvents(name, targetType, depth + 1);
         }

         else if (tagToProperty.find(tag) != tagToProperty.end()) {

            std::string value = parseString();

            ast->appendChild(ASTSetProperty(
               targetType,
               tagToProperty[tag],
               value,
               xmlTextReaderGetParserLineNumber(reader),
               name
            ));

            checkClosingTag(tag);
         }

         else {
            throw ParseException(std::string("invalid tag <") + tag
               + "> in room or class definition");
         }
      }
   }

   /***************************************************************************/

   void XMLParser::parseRoomConnection(std::string direction, std::string roomName,
   std::string connectTo, std::string targetType) {

      ast->appendChild(ASTConnectRooms(
         targetType,
         roomName,
         connectTo,
         direction,
         xmlTextReaderGetParserLineNumber(reader)
      ));

      checkClosingTag(direction);
   }

   /***************************************************************************/

   void XMLParser::parseRoomContains(std::string roomName, std::string targetType) {

      while (nextTag() && 4 == getDepth()) {

         std::string tag = getTagName();

         if (0 == tag.compare("object") || 0 == tag.compare("creature")) {

            // We can't add objects or creatures to a room class because that
            // doesn't make sense (can only be inserted into a single specific
            // room.)
            if (0 == targetType.compare("entity")) {

               std::string thingName = parseString();

               ast->appendChild(ASTInsertIntoRoom(
                  thingName,
                  roomName,
                  xmlTextReaderGetParserLineNumber(reader)
               ));
            }

            else {
               throw ParseException("Objects and creatures must be placed into a room and cannot be inserted into a room class.");
            }
         }

         else {
            throw ParseException(std::string("invalid tag <") + getTagName()
               + "> in <rooms>");
         }

         checkClosingTag(tag);
      }

      checkClosingTag("contains");
   }

   /***************************************************************************/

   void XMLParser::parseCreatureAutoAttack(std::string creatureName, std::string targetType,
   int depth) {

      static std::unordered_map<std::string, std::string> tagToProperty({
         {"enabled", "autoattack.enabled"}, {"repeat", "autoattack.repeat"},
         {"interval", "autoattack.interval"}
      });

      while (nextTag() && depth == getDepth()) {

         std::string tag = getTagName();

         if (tagToProperty.find(tag) != tagToProperty.end()) {

            std::string value = parseString();

            ast->appendChild(ASTSetProperty(
               targetType,
               tagToProperty[tag],
               value,
               xmlTextReaderGetParserLineNumber(reader),
               creatureName
            ));

            checkClosingTag(tag);
         }

         else {
            throw ParseException(std::string("invalid tag <") + getTagName()
               + "> in creature autoattack section");
         }
      }

      checkClosingTag("autoattack");
   }

   /***************************************************************************/

   void XMLParser::parseCreatureWandering(std::string creatureName, std::string targetType) {

      static std::unordered_map<std::string, std::string> tagToProperty({
         {"enabled", "wandering.enabled"}, {"interval", "wandering.interval"},
         {"wanderlust", "wandering.wanderlust"}
      });

      while (nextTag() && 4 == getDepth()) {

         std::string tag = getTagName();

         if (tagToProperty.find(tag) != tagToProperty.end()) {

            std::string value = parseString();

            ast->appendChild(ASTSetProperty(
               targetType,
               tagToProperty[tag],
               value,
               xmlTextReaderGetParserLineNumber(reader),
               creatureName
            ));

            checkClosingTag(tag);
         }

         else {
            throw ParseException(std::string("invalid tag <") + tag
               + "> in creature wandering settings");
         }
      }

      checkClosingTag("wandering");
   }

   /***************************************************************************/

   void XMLParser::parseBeingRespawn(std::string beingName, std::string targetType, int depth) {

      static std::unordered_map<std::string, std::string> tagToProperty({
         {"enabled", "respawn.enabled"}, {"interval", "respawn.interval"},
         {"lives", "respawn.lives"}
      });

      while (nextTag() && depth == getDepth()) {

         std::string tag = getTagName();

         if (tagToProperty.find(tag) != tagToProperty.end()) {

            std::string value = parseString();

            ast->appendChild(ASTSetProperty(
               targetType,
               tagToProperty[tag],
               value,
               xmlTextReaderGetParserLineNumber(reader),
               beingName
            ));

            checkClosingTag(tag);
         }

         else {
            throw ParseException(std::string("invalid tag <") + tag
               + "> in being respawn section");
         }
      }

      checkClosingTag("respawn");
   }

   /***************************************************************************/

   void XMLParser::parseBeingInventory(std::string beingName, std::string targetType,
   bool allowObjects) {

      static std::unordered_map<std::string, std::string> tagToProperty({
         {"weight", "inventory.weight"}
      });

      while (nextTag() && 4 == getDepth()) {

         std::string tag = getTagName();

         if (0 == tag.compare("object")) {

            std::string value = parseString();

            if (0 == targetType.compare("entity")) {

               ast->appendChild(ASTInsertIntoInventory(
                  value,
                  beingName,
                  xmlTextReaderGetParserLineNumber(reader)
               ));

               checkClosingTag(tag);
            }

            // We can't add items to a Being class's inventory because multiple
            // Beings can be instantiated from it and an Object can only be in
            // one Being's inventory at a time.
            else if (0 == targetType.compare("class")) {
               throw ParseException("Objects cannot be placed into a class's inventory.");
            }

            // We can't add items to the default player's inventory for the same
            // reason as above.
            else {
               throw ParseException("Objects cannot be placed into the default player's inventory.");
            }
         }

         else if (tagToProperty.find(tag) != tagToProperty.end()) {

            std::string value = parseString();

            ast->appendChild(ASTSetProperty(
               targetType,
               tagToProperty[tag],
               value,
               xmlTextReaderGetParserLineNumber(reader),
               beingName
            ));

            checkClosingTag(tag);
         }

         else {
            throw ParseException(std::string("invalid tag <") + tag
               + "> in inventory settings");
         }
      }

      checkClosingTag("inventory");
   }

   /***************************************************************************/

   void XMLParser::parseBeingAttributes(std::string beingName, std::string targetType) {

      while (nextTag() && 4 == getDepth()) {

        std::string tag = getTagName();

         // TODO: should I allow any tag here? Custom attribute values are
         // something I'd like to support in the future, and it would make error
         // checking much simpler. At worst, custom values would just be ignored
         // during gameplay until they're supported.
         if (
            0 == tag.compare("strength") ||
            0 == tag.compare("dexterity") ||
            0 == tag.compare("intelligence")
         ) {

            std::string value = parseString();

            ast->appendChild(ASTSetAttribute(
               targetType,
               tag,
               value,
               xmlTextReaderGetParserLineNumber(reader),
               beingName
            ));

            checkClosingTag(tag);
         }

         else {
            throw ParseException(std::string("invalid tag <") + tag
               + "> in default player's inventory settings");
         }
      }

      checkClosingTag("attributes");
   }

   /***************************************************************************/

   void XMLParser::parseThingAliases(std::string entityName, std::string targetType, int depth) {

      while (nextTag() && depth == getDepth()) {

         if (0 == getTagName().compare("alias")) {

            std::string alias = parseString();

            ast->appendChild(ASTSetAlias(
               targetType,
               alias,
               entityName,
               xmlTextReaderGetParserLineNumber(reader)
            ));

            checkClosingTag("alias");
         }

         else {
            throw ParseException(std::string("invalid tag <") + getTagName()
               + "> in <aliases>");
         }
      }

      checkClosingTag("aliases");
   }

   /***************************************************************************/

   std::string XMLParser::parseString() {

      std::string value = getNodeValue();
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

   std::string XMLParser::getAttribute(const char *name) {

      const char *attr;

      attr = (const char *)xmlTextReaderGetAttribute(reader, (xmlChar *)name);

      if (!attr) {
         throw ParseException(std::string("missing attribute '") + name);
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

   void XMLParser::checkClosingTag(std::string tag) {

      // check to see if nextTag() encountered a closing tag
      if (lastClosedTag.length() > 0) {

         if (0 != lastClosedTag.compare(tag)) {
            throw ParseException(std::string("expected closing </") + tag + ">");
         }

         lastClosedTag = "";
         return;
      }

      // TODO: skip over XML comments (right now, we'll get an error!)

      if (xmlTextReaderRead(reader) < 0) {
         throw ParseException(std::string("Unknown error when checking closing tag </") + tag + ">");
      }

      else if (XML_ELEMENT_DECL != xmlTextReaderNodeType(reader) ||
      0 != tag.compare(strToLower((const char *)xmlTextReaderConstName(reader)))) {
         throw ParseException(std::string("missing closing </") + tag + ">");
      }
   }
}
