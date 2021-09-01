#include <memory>

#include <trogdor/utility.h>
#include <trogdor/filesystem.h>

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

   void XMLParser::declareEntity(
      std::string name,
      std::string className,
      entity::EntityType type,
      std::optional<std::string> plural,
      int lineno
   ) {

      // What can I say? I like grammatically correct error messages.
      static std::unordered_set<char> vowels = {'a', 'e', 'i', 'o', 'u'};

      if (declaredEntities.end() != declaredEntities.find(name)) {

         std::string typeStr = entity::Entity::typeToStr(declaredEntities[name].type);
         std::string article = vowels.end() != vowels.find(typeStr[0]) ?
            std::string("an") : std::string("a");

         throw ParseException(article + " " + typeStr + " named '"
            + name + "' was already defined on line " +
            std::to_string(declaredEntities[name].lineno) +
            ". Please ensure that all entity names are unique.");
      }

      // Primitive class names ("room", "object", etc.) are an exception to the
      // rule that the class has to have been previously defined because it's
      // already defined internally
      else if (
         entity::ENTITY_UNDEFINED == entity::Entity::strToType(className) &&
         declaredEntityClasses.end() == declaredEntityClasses.find(className)
      ) {

         std::string article = vowels.end() != vowels.find(entity::Entity::typeToStr(type)[0]) ?
            std::string("an") : std::string("a");

         throw ParseException(article + " " + entity::Entity::typeToStr(type) + " named '"
            + name + "' was defined according to the '" + className
            + "' class, but that class hasn't been defined.");
      }

      // Same note as above about primitive class names
      else if (
         entity::ENTITY_UNDEFINED == entity::Entity::strToType(className) &&
         type != declaredEntityClasses[className]
      ) {

         std::string article = vowels.end() != vowels.find(entity::Entity::typeToStr(type)[0]) ?
            std::string("an") : std::string("a");

         throw ParseException(article + " " + Entity::typeToStr(type) + " named '"
            + name + "' was defined according to the '" + className
            + "' class, but that class is for " +
            entity::Entity::typeToStr(declaredEntityClasses[className]) + "s.");
      }

      // An implicit definition was already made, so all we have to do is update
      // that AST node to make sure it contains the correct class and then
      // remove the unresolved reference.
      if (unresolvedEntityReferences.end() != unresolvedEntityReferences.find(name)) {

         unresolvedEntityReferences[name].first->getChildren()[2]->updateValue(className);

         // Resource types might also have a fourth optional AST parameter to
         // specify a custom plural name
         if (plural) {
            unresolvedEntityReferences[name].first->appendChild(
               std::make_shared<ASTNode>(
                  *plural,
                  AST_VALUE,
                  lineno
               )
            );
         }

         unresolvedEntityReferences.erase(name);
      }

      else {
         insertDefineEntityOperation(
            name,
            className,
            type,
            lineno,
            plural
         );
      }

      declaredEntities[name] = {
         lineno,
         name,
         className,
         type
      };
   }

   /***************************************************************************/

   void XMLParser::setUnresolvedEntityReference(
      std::string name,
      std::string className,
      entity::EntityType type,
      int lineno
   ) {

      if (declaredEntities.end() == declaredEntities.find(name)) {

         if (unresolvedEntityReferences.end() == unresolvedEntityReferences.find(name)) {

            unresolvedEntityReferences[name] = {
               insertDefineEntityOperation(
                  name,
                  entity::Entity::typeToStr(type),
                  type,
                  lineno
               ), {}
            };
         }

         unresolvedEntityReferences[name].second.push_back(lineno);
      }
   }

   /***************************************************************************/

   std::shared_ptr<ASTNode> XMLParser::insertDefineEntityOperation(
      std::string name,
      std::string className,
      entity::EntityType type,
      int lineno,
      std::optional<std::string> plural
   ) {

      std::string defaultTitle = entity::ENTITY_OBJECT == type ?
         std::string("a ") + name : name;

      std::shared_ptr<ASTNode> definitionNode = ast->appendChild(ASTDefineEntity(
         name,
         entity::Entity::typeToStr(type),
         className,
         plural,
         lineno
      ));

      // Set entity's default title
      ast->appendChild(ASTSetProperty(
         "entity",
         "title",
         defaultTitle,
         xmlTextReaderGetParserLineNumber(reader),
         name
      ));

      return definitionNode;
   }

   /***************************************************************************/

   void XMLParser::parse(std::string filename) {

      // Keeping track of this will help us later when we need to locate Lua
      // script files with relative paths
      gamefilePath = filename;
      trim(gamefilePath);

      if (reader) {
         xmlFreeTextReader(reader);
         reader = nullptr;
      }

      reader = xmlReaderForFile(gamefilePath.c_str(), NULL, XML_PARSE_NOBLANKS);
      if (NULL == reader) {
         throw ParseException(
            std::string("failed to open ") +
            STD_FILESYSTEM::path(gamefilePath).filename().string()
         );
      }

      try {

         // This has to be inside a try block because if the XML file is
         // malformatted (like, for example, there being more than one root
         // element), nextTag() will throw an exception that would otherwise
         // go uncaught.
         if (!nextTag()) {
            throw ParseException(gamefilePath + ": empty XML file");
         }

         else if (0 != getTagName().compare("game")) {
            throw ParseException("expected <game>");
         }

         parseGame();
      }

      catch (const Exception &e) {
         throw ParseException(e.what(), gamefilePath,
            xmlTextReaderGetParserLineNumber(reader));
      }

      // If there are unresolved entity references, the game.xml is incomplete
      // and we should throw an exception.
      if (unresolvedEntityReferences.size()) {

         std::string errorMsg = "The following entity references are undefined:";

         for (const auto &reference: unresolvedEntityReferences) {

            std::string name = reference.first;
            std::string type = reference.second.first->getChildren()[1]->getValue();
            std::string lines = reference.second.second.size() > 1 ? "lines " : "line ";

            for (unsigned i = 0; i < reference.second.second.size(); i++) {

               lines += std::to_string(reference.second.second[i]);

               if (i < reference.second.second.size() - 1) {
                  lines += i < reference.second.second.size() - 2 ? ", " : ", and ";
               }
            }

            errorMsg += "\n\t" + type + " '" + name + "' on " + lines;
         }

         throw ParseException(errorMsg, gamefilePath, 0);
      }

      instantiator->instantiate(ast);
   }

   /***************************************************************************/

   void XMLParser::parseGame() {

      // parse the remaining sections
      while (nextTag() && 1 == getDepth()) {

         if (0 == getTagName().compare("classes")) {
            parseClasses();
         }

         else if (0 == getTagName().compare("vocabulary")) {
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

         else if (0 == getTagName().compare("resources")) {
            parseResources();
         }

         else {
            throw ParseException(std::string("invalid section: <") + getTagName() + ">");
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

         else if (0 == getTagName().compare("resources")) {
            parseClassesResources();
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
         parseClassesRoom();
      }

      checkClosingTag("rooms");
   }

   /***************************************************************************/

   void XMLParser::parseClassesRoom() {

      std::string name = getTagName();

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
      checkClosingTag(name);
   }

   /***************************************************************************/

   void XMLParser::parseClassesObjects() {

      while (nextTag() && 3 == getDepth()) {
         parseClassesObject();
      }

      checkClosingTag("objects");
   }

   /***************************************************************************/

   void XMLParser::parseClassesObject() {

      std::string name = getTagName();

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
      checkClosingTag(name);
   }

   /***************************************************************************/

   void XMLParser::parseClassesCreatures() {

      while (nextTag() && 3 == getDepth()) {
         parseClassesCreature();
      }

      checkClosingTag("creatures");
   }

   /***************************************************************************/

   void XMLParser::parseClassesCreature() {

      std::string name = getTagName();

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
      checkClosingTag(name);
   }

   /***************************************************************************/

   void XMLParser::parseClassesResources() {

      while (nextTag() && 3 == getDepth()) {
         parseClassesResource();
      }

      checkClosingTag("resources");
   }

   /***************************************************************************/

   void XMLParser::parseClassesResource() {

      std::string name = getTagName();

      if (isClassNameReserved(name)) {
         throw ParseException(std::string("class name '") + name + "' is reserved");
      }

      declaredEntityClasses[name] = entity::ENTITY_RESOURCE;

      ast->appendChild(ASTDefineEntityClass(
         name,
         entity::Entity::typeToStr(entity::ENTITY_RESOURCE),
         xmlTextReaderGetParserLineNumber(reader)
      ));

      parseResourceProperties(name, "class", 4);
      checkClosingTag(name);
   }

   /***************************************************************************/

   void XMLParser::parseIntroduction() {

      static std::unordered_map<std::string, std::string> tagToProperty({
         {"enabled", "introduction.enabled"}, {"text", "introduction.text"}
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
            trim(action);

            std::string synonym = parseString();
            trim(synonym);

            ast->appendChild(ASTDefineVerbSynonym(
               action,
               synonym,
               xmlTextReaderGetParserLineNumber(reader)
            ));

            checkClosingTag("verb");
         }

         else if (0 == getTagName().compare("direction")) {

            std::string direction = getAttribute("direction");
            trim(direction);

            std::string synonym = parseString();
            trim(synonym);

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

   void XMLParser::parseMessages(std::string entityName, std::string targetType, int depth) {

      while (nextTag() && depth == getDepth()) {

         if (0 == getTagName().compare("message")) {

            std::string messageName = getAttribute("name");
            std::string message = parseString();

            trim(messageName);

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

         trim(script);

         // If the gamefile has a parent path and the script path is relative,
         // we should look for the script relative to the gamefile's location
         std::string gamefileLocation = STD_FILESYSTEM::path(gamefilePath).parent_path();
         if (gamefileLocation.length() > 0 && script[0] != STD_FILESYSTEM::path::preferred_separator) {
            script = gamefileLocation + STD_FILESYSTEM::path::preferred_separator + script;
         }
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

      declareEntity(
         name,
         className,
         entity::ENTITY_OBJECT,
         std::nullopt,
         xmlTextReaderGetParserLineNumber(reader)
      );

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

         else if (0 == tag.compare("resources")) {

            if (0 != targetType.compare("entity")) {
               throw ParseException("Cannot allocate resources to a class");
            }

            parseTangibleResources(name, depth + 1);
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
         {"health", "health"}, {"maxhealth", "maxhealth"},
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

      declareEntity(
         name,
         className,
         entity::ENTITY_CREATURE,
         std::nullopt,
         xmlTextReaderGetParserLineNumber(reader)
      );

      parseCreatureProperties(name, "entity", 3);
      checkClosingTag(className);
   }

   /***************************************************************************/

   void XMLParser::parseCreatureProperties(std::string name, std::string targetType, int depth) {

      static std::unordered_map<std::string, std::string> tagToProperty({
         {"title", "title"}, {"description", "longDesc"}, {"short", "shortDesc"},
         {"health", "health"}, {"maxhealth", "maxhealth"},
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

         else if (0 == tag.compare("resources")) {

            if (0 != targetType.compare("entity")) {
               throw ParseException("Cannot allocate resources to a class");
            }

            parseTangibleResources(name, depth + 1);
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
      ast->appendChild(ASTSetProperty(
         targetType,
         "counterattack.default",
         "",
         xmlTextReaderGetParserLineNumber(reader),
         name
      ));
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

      declareEntity(
         name,
         className,
         entity::ENTITY_ROOM,
         std::nullopt,
         xmlTextReaderGetParserLineNumber(reader)
      );

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
            std::optional<std::string> description = getOptionalAttribute("description");
            std::string connection = parseString();
            parseRoomConnection(tag, name, connection, description, targetType);
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

         else if (0 == tag.compare("resources")) {

            if (0 != targetType.compare("entity")) {
               throw ParseException("Cannot allocate resources to a class");
            }

            parseTangibleResources(name, depth + 1);
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
   std::string connectTo, std::optional<std::string> description, std::string targetType) {

      setUnresolvedEntityReference(
         connectTo,
         entity::Entity::typeToStr(entity::ENTITY_ROOM),
         entity::ENTITY_ROOM,
         xmlTextReaderGetParserLineNumber(reader)
      );

      ast->appendChild(ASTConnectRooms(
         targetType,
         roomName,
         connectTo,
         direction,
         description,
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

               setUnresolvedEntityReference(
                  thingName,
                  tag,
                  entity::Entity::strToType(tag),
                  xmlTextReaderGetParserLineNumber(reader)
               );

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

               setUnresolvedEntityReference(
                  value,
                  tag,
                  entity::Entity::strToType(tag),
                  xmlTextReaderGetParserLineNumber(reader)
               );

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

   void XMLParser::parseResources() {

      while (nextTag() && 2 == getDepth()) {

         if (0 == getTagName().compare("resource")) {
            parseResource();
         }

         else if (entityClassDeclared(getTagName(), entity::ENTITY_RESOURCE)) {
            parseResource(getTagName());
         }

         else {
            throw ParseException(std::string("invalid tag <") + getTagName()
               + "> in <resources>");
         }
      }

      checkClosingTag("resources");
   }

   /***************************************************************************/

   void XMLParser::parseResource(std::string className) {

      std::string name = getAttribute("name");
      std::optional<std::string> plural = getOptionalAttribute("plural");

      declareEntity(
         name,
         className,
         entity::ENTITY_RESOURCE,
         plural,
         xmlTextReaderGetParserLineNumber(reader)
      );

      parseResourceProperties(name, "entity", 3);
      checkClosingTag(className);
   }

   /***************************************************************************/

   void XMLParser::parseResourceProperties(std::string name, std::string targetType, int depth) {

      static std::unordered_map<std::string, std::string> tagToProperty({
         {"title", "title"}, {"pluraltitle", "pluralTitle"},
         {"description", "longDesc"}, {"short", "shortDesc"},
         {"integer", "requireIntegerAllocations"}, {"amount", "amountAvailable"},
         {"maxallocation", "maxAmountPerDepositor"}
      });

      while (nextTag() && depth == getDepth()) {

         std::string tag = getTagName();

         if (0 == tag.compare("meta")) {
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
               + "> in resource or class definition");
         }
      }
   }

   /***************************************************************************/

   void XMLParser::parseTangibleResources(std::string entityName, int depth) {

      while (nextTag() && depth == getDepth()) {

         if (0 == getTagName().compare("resource")) {

            std::string resourceName = getAttribute("name");
            std::string resourceAmount = parseString();

            trim(resourceAmount);

            if (!resourceAmount.length()) {
               throw ParseException("resource allocation: expected numeric value but got an empty string");
            }

            setUnresolvedEntityReference(
               resourceName,
               entity::Entity::typeToStr(entity::ENTITY_RESOURCE),
               entity::ENTITY_RESOURCE,
               xmlTextReaderGetParserLineNumber(reader)
            );

            ast->appendChild(ASTAllocateResource(
               entityName,
               resourceName,
               resourceAmount,
               xmlTextReaderGetParserLineNumber(reader)
            ));

            checkClosingTag("resource");
         }

         else {
            throw ParseException("expected <resource> in <resources>");
         }
      }

      checkClosingTag("resources");
   }

   /***************************************************************************/

   std::string XMLParser::parseString() {

      std::string value = getNodeValue();
      trim(value);

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

   std::optional<std::string> XMLParser::getOptionalAttribute(const char *name) {

      const char *attr;

      attr = (const char *)xmlTextReaderGetAttribute(reader, (xmlChar *)name);

      if (!attr) {
         return std::nullopt;
      }

      return attr;
   }

   /***************************************************************************/

   std::string XMLParser::getAttribute(const char *name) {

      std::optional<std::string> attr = getOptionalAttribute(name);

      if (!attr) {
         throw ParseException(std::string("missing attribute '") + name);
      }

      return *attr;
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
