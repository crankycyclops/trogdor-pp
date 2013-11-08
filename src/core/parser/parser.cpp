#include "../include/parser/parser.h"

#include "../include/iostream/nullout.h"
#include "../include/iostream/placeout.h"
#include "../include/iostream/streamin.h"
#include "../include/iostream/streamout.h"

#include "../include/timer/jobs/wander.h"

using namespace std;

namespace core {


   Parser::Parser(Game *g, string gameFile) {

      game = g;
      filename = gameFile;

      reader = xmlReaderForFile(gameFile.c_str(), NULL, XML_PARSE_NOBLANKS);
      if (NULL == reader) {
         throw "failed to open " + gameFile + "!\n";
      }

      gameL = new LuaState();
      eventListener = new event::EventListener();
      defaultPlayer = new entity::Player(game, "default", new NullOut(),
         new StreamIn(&cin), new StreamOut(&cerr));
   }

   /***************************************************************************/

   Parser::~Parser() {

      // TODO: do I free entity objects inside, or let Game do it?
      xmlFreeTextReader(reader);
      // TODO: do I free gameL and eventListener, or let Game do it?
      // TODO: do I need to free default player object, or let Game do it?
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

      Room *room = new Room(game, name, new PlaceOut(), new StreamIn(&cin),
         new StreamOut(&cerr));

      // for type checking
      room->setClass(name);
      room->setTitle(name);

      try {
         typeClasses.insertType(name, room);
      }

      catch (bool e) {
            s << filename << ": room class '" << name << "' already defined (line "
               << xmlTextReaderGetParserLineNumber(reader) << ")";
            throw s.str();
      }

      parseRoomProperties(room, 4);
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

      Object *object = new Object(game, name, new NullOut(), new StreamIn(&cin),
         new StreamOut(&cerr));

      // for type checking
      object->setClass(name);
      object->setTitle(name);

      try {
         typeClasses.insertType(name, object);
      }

      catch (bool e) {
            s << filename << ": object class '" << name << "' already defined (line "
               << xmlTextReaderGetParserLineNumber(reader) << ")";
            throw s.str();
      }

      parseObjectProperties(object, 4);
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

      Creature *creature = new Creature(game, name, new NullOut(),
         new StreamIn(&cin), new StreamOut(&cerr));

      // for type checking
      creature->setClass(name);
      creature->setTitle(name);

      try {
         typeClasses.insertType(name, creature);
      }

      catch (bool e) {
            s << filename << ": creature class '" << name << "' already defined (line "
               << xmlTextReaderGetParserLineNumber(reader) << ")";
            throw s.str();
      }

      parseCreatureProperties(creature, 4);
      checkClosingTag("creature");
   }

   /***************************************************************************/

   void Parser::parseIntroduction() {

      stringstream s;

      while (nextTag() && 2 == getDepth()) {

         if (0 == getTagName().compare("enabled")) {
            parseIntroductionEnabled();
         }

         else if (0 == getTagName().compare("pause")) {
            parseIntroductionPause();
         }

         else if (0 == getTagName().compare("text")) {
            parseIntroductionText();
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

   void Parser::parseIntroductionEnabled() {

      game->setIntroductionEnabled(parseBool());
      checkClosingTag("enabled");
   }

   /***************************************************************************/

   void Parser::parseIntroductionPause() {

      game->setIntroductionPause(parseBool());
      checkClosingTag("pause");
   }

   /***************************************************************************/

   void Parser::parseIntroductionText() {

      game->setIntroductionText(parseString());
      checkClosingTag("text");
   }

   /***************************************************************************/

   void Parser::parseGameMeta() {

      while (nextTag() && 2 == getDepth()) {
         parseGameMetaValue(getTagName());
      }

      checkClosingTag("meta");
   }

   /***************************************************************************/

   void Parser::parseGameMetaValue(string key) {

      string value = parseString();
      game->setMeta(key, value);
      checkClosingTag(key);
   }

   /***************************************************************************/

   void Parser::parseEntityMeta(Entity *entity, int depth) {

      while (nextTag() && depth == getDepth()) {
         parseEntityMetaValue(getTagName(), entity);
      }

      checkClosingTag("meta");
   }

   /***************************************************************************/

   void Parser::parseEntityMetaValue(string key, Entity *entity) {

      string value = parseString();
      entity->setMeta(key, value);
      checkClosingTag(key);
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

         else if (typeClasses.roomTypeExists(getTagName())) {
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

      if (entities.isset(name)) {
         stringstream s;
         s << "Cannot define room with name '" << name << ": an entity with " <<
            "that name already exists";
         throw s.str();
      }

      Room *room = new Room(game, name, new PlaceOut(), new StreamIn(&cin),
         new StreamOut(&cerr));

      room->setClass(className);

      entities.set(name, room);
      places.set(name, room);
      rooms.set(name, room);

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

         else if (typeClasses.objectTypeExists(getTagName())) {
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

      if (entities.isset(name)) {
         stringstream s;
         s << "Cannot define object with name '" << name << ": an entity with "
            << "that name already exists";
         throw s.str();
      }

      Object *object;

      if (0 == className.compare("object")) {
         object = new Object(game, name, new NullOut(), new StreamIn(&cin), new StreamOut(&cerr));
      }

      else {
         object = new Object(*typeClasses.getObjectType(className), name);
      }

      object->setClass(className);

      entities.set(name, object);
      things.set(name, object);
      items.set(name, object);
      objects.set(name, object);

      checkClosingTag(className);
   }

   /***************************************************************************/

   void Parser::parseManifestCreatures() {

      stringstream s;

      while (nextTag() && 3 == getDepth()) {

         if (0 == getTagName().compare("creature")) {
            parseManifestCreature();
         }

         else if (typeClasses.creatureTypeExists(getTagName())) {
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

      if (entities.isset(name)) {
         stringstream s;
         s << "Cannot define creature with name '" << name << ": an entity "
            << "with that name already exists";
         throw s.str();
      }

      Creature *creature = new Creature(game, name, new NullOut(),
         new StreamIn(&cin), new StreamOut(&cerr));

      creature->setClass(className);

      entities.set(name, creature);
      things.set(name, creature);
      beings.set(name, creature);
      creatures.set(name, creature);

      checkClosingTag(className);
   }

   /***************************************************************************/

   Messages *Parser::parseMessages(int depth) {

      stringstream s;
      Messages *m = new Messages;

      while (nextTag() && depth == getDepth()) {

         if (0 == getTagName().compare("message")) {
            parseMessage(m);
         }

         else {
            s << filename << ": invalid tag <" << getTagName() << "> in "
               << "messages section -- expected <message> (line "
               << xmlTextReaderGetParserLineNumber(reader) << ")";
            throw s.str();
         }
      }

      checkClosingTag("messages");
      return m;
   }

   /***************************************************************************/

   void Parser::parseMessage(Messages *m) {

      string name = getAttribute("name");
      m->set(name, getNodeValue());
      checkClosingTag("message");
   }

   /***************************************************************************/

   void Parser::parseEvents(LuaState *L, EventListener *triggers, int depth) {

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

   void Parser::parseScript(LuaState *L) {

      // external script
      try {
         L->loadScriptFromFile(getAttribute("src"));
      }

      // inline script
      catch (string error) {
         L->loadScriptFromString(parseString());
      }

      checkClosingTag("script");
   }

   /***************************************************************************/

   void Parser::parseEvent(LuaState *L, EventListener *triggers) {

      string name = getAttribute("name");
      string function = parseString();

      LuaEventTrigger *trigger = new LuaEventTrigger(function, L);
      triggers->add(name.c_str(), trigger);

      checkClosingTag("event");
   }

   /***************************************************************************/

   void Parser::parseObjects() {

      stringstream s;

      while (nextTag() && 2 == getDepth()) {

         if (0 == getTagName().compare("object")) {
            parseObject();
         }

         else if (typeClasses.objectTypeExists(getTagName())) {
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

      Object *object = objects.get(getAttribute("name"));

      if (0 == object) {
         s << filename << ": object '" << getAttribute("name") << "' was not "
            << "declared in the manifest (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      // type checking
      else if (className != object->getClass()) {
         s << filename << ": object type mismatch: '" << getAttribute("name")
            << "' is of type " << object->getClass() << ", but was declared in "
            << "objects section to be of type " << className << " (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      // set the object's default title
      s << "a " << getAttribute("name");
      object->setTitle(s.str());

      parseObjectProperties(object, 3);
      checkClosingTag(className);
   }

   /***************************************************************************/

   void Parser::parseObjectProperties(Object *object, int depth) {

      stringstream s;

      while (nextTag() && depth == getDepth()) {

         if (0 == getTagName().compare("title")) {
            object->setTitle(parseEntityTitle());
         }

         else if (0 == getTagName().compare("description")) {
            object->setLongDescription(parseEntityLongDescription());
         }

         else if (0 == getTagName().compare("short")) {
            object->setShortDescription(parseEntityShortDescription());
         }

         else if (0 == getTagName().compare("weight")) {
            object->setWeight(parseObjectWeight());
         }

         else if (0 == getTagName().compare("takeable")) {
            object->setTakeable(parseItemTakeable());
         }

         else if (0 == getTagName().compare("droppable")) {
            object->setDroppable(parseItemDroppable());
         }

         else if (0 == getTagName().compare("weapon")) {
            object->setIsWeapon(parseItemWeapon());
         }

         else if (0 == getTagName().compare("damage")) {
            object->setDamage(parseItemDamage());
         }

         else if (0 == getTagName().compare("events")) {
            parseEvents(object->L, object->triggers, depth + 1);
         }

         else if (0 == getTagName().compare("aliases")) {
            parseThingAliases(object, depth + 1);
         }

         else if (0 == getTagName().compare("meta")) {
            parseEntityMeta(object, depth + 1);
         }

         else if (0 == getTagName().compare("messages")) {
            Messages *m = parseMessages(depth + 1);
            object->setMessages(*m);
            delete m;
         }

         else {
            s << filename << ": invalid tag <" << getTagName() << "> in "
               << "object or object class definition (line "
               << xmlTextReaderGetParserLineNumber(reader) << ")";
            throw s.str();
         }
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

      while (nextTag() && 3 == getDepth()) {

         if (0 == getTagName().compare("messages")) {
            Messages *m = parseMessages(4);
            defaultPlayer->setMessages(*m);
            delete m;
         }

         else if (0 == getTagName().compare("inventory")) {
            parseBeingInventory(defaultPlayer, false);
         }

         else if (0 == getTagName().compare("respawn")) {
            Parser::parseBeingRespawn(defaultPlayer, 4);
         }

         else if (0 == getTagName().compare("attributes")) {
            parseBeingAttributes(defaultPlayer);
         }

         else if (0 == getTagName().compare("alive")) {
            defaultPlayer->setAlive(parseBeingAlive());
         }

         else if (0 == getTagName().compare("health")) {
            defaultPlayer->setHealth(parseBeingHealth());
         }

         else if (0 == getTagName().compare("maxhealth")) {
            defaultPlayer->setMaxHealth(parseBeingMaxHealth());
         }

         else if (0 == getTagName().compare("woundrate")) {
            defaultPlayer->setWoundRate(parseBeingWoundRate());
         }

         else if (0 == getTagName().compare("attackable")) {
            defaultPlayer->setAttackable(parseBeingAttackable());
         }

         else if (0 == getTagName().compare("damagebarehands")) {
            defaultPlayer->setDamageBareHands(parseBeingDamageBareHands());
         }

         else {
            s << filename << ": invalid tag <" << getTagName() << "> in "
               << "default player settings (line "
               << xmlTextReaderGetParserLineNumber(reader) << ")";
            throw s.str();
         }
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

      Creature *creature = creatures.get(getAttribute("name"));

      if (0 == creature) {
         s << filename << ": creature '" << getAttribute("name") << "' was not "
            << "declared in the manifest (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      // type checking
      else if (className != creature->getClass()) {
         s << filename << ": creature type mismatch: '" << getAttribute("name")
            << "' is of type " << creature->getClass() << ", but was declared in "
            << "creatures section to be of type " << className << " (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      // set the creature's default title
      creature->setTitle(getAttribute("name"));

      parseCreatureProperties(creature, 3);

      // if wandering was enabled, insert a timer job for it
      if (creature->getWanderEnabled()) {
         game->insertTimerJob(new WanderTimerJob(game, creature->getWanderInterval(),
            -1, creature->getWanderInterval(), creature));
      }

      checkClosingTag(className);
   }

   /***************************************************************************/

   void Parser::parseCreatureProperties(Creature *creature, int depth) {

      stringstream s;
      bool counterAttackParsed = false;

      while (nextTag() && depth == getDepth()) {

         if (0 == getTagName().compare("title")) {
            creature->setTitle(parseEntityTitle());
         }

         else if (0 == getTagName().compare("description")) {
            creature->setLongDescription(parseEntityLongDescription());
         }

         else if (0 == getTagName().compare("short")) {
            creature->setShortDescription(parseEntityShortDescription());
         }

         else if (0 == getTagName().compare("alive")) {
            creature->setAlive(parseBeingAlive());
         }

         else if (0 == getTagName().compare("health")) {
            creature->setHealth(parseBeingHealth());
         }

         else if (0 == getTagName().compare("maxhealth")) {
            creature->setMaxHealth(parseBeingMaxHealth());
         }

         else if (0 == getTagName().compare("attackable")) {
            creature->setAttackable(parseBeingAttackable());
         }

         else if (0 == getTagName().compare("woundrate")) {
            creature->setWoundRate(parseBeingWoundRate());
         }

         else if (0 == getTagName().compare("damagebarehands")) {
            creature->setDamageBareHands(parseBeingDamageBareHands());
         }

         else if (0 == getTagName().compare("allegiance")) {
            creature->setAllegiance(parseCreatureAllegiance());
         }

         else if (0 == getTagName().compare("counterattack")) {
            counterAttackParsed = true;
            creature->setCounterAttack(parseCreatureCounterAttack());
         }

         else if (0 == getTagName().compare("respawn")) {
            Parser::parseBeingRespawn(creature, depth + 1);
         }

         else if (0 == getTagName().compare("autoattack")) {
            parseCreatureAutoAttack(creature, depth + 1);
         }

         else if (0 == getTagName().compare("wandering")) {
            parseCreatureWandering(creature);
         }

         else if (0 == getTagName().compare("inventory")) {
            parseBeingInventory(creature, true);
         }

         else if (0 == getTagName().compare("attributes")) {
            parseBeingAttributes(creature);
         }

         else if (0 == getTagName().compare("events")) {
            parseEvents(creature->L, creature->triggers, depth + 1);
         }

         else if (0 == getTagName().compare("aliases")) {
            parseThingAliases(creature, depth + 1);
         }

         else if (0 == getTagName().compare("meta")) {
            parseEntityMeta(creature, depth + 1);
         }

         else if (0 == getTagName().compare("messages")) {
            Messages *m = parseMessages(depth + 1);
            creature->setMessages(*m);
            delete m;
         }

         else {
            s << filename << ": invalid tag <" << getTagName() << "> in "
               << "creature definition (line "
               << xmlTextReaderGetParserLineNumber(reader) << ")";
            throw s.str();
         }
      }

      // verify default counter-attack rules
      if (!counterAttackParsed) {

         switch (creature->getAllegiance()) {

            case entity::Creature::FRIEND:
               creature->setCounterAttack(false);
               break;

            default:
               creature->setCounterAttack(true);
               break;
         }
      }
   }

   /***************************************************************************/

   void Parser::parseRooms() {

      stringstream s;

      while (nextTag() && 2 == getDepth()) {

         if (0 == getTagName().compare("room")) {
            parseRoom();
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

      Room *room = rooms.get(getAttribute("name"));

      if (0 == room) {
         s << filename << ": room '" << getAttribute("name") << "' was not "
            << "declared in the manifest (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      // type checking
      else if (className != room->getClass()) {
         s << filename << ": room type mismatch: '" << getAttribute("name")
            << "' is of type " << room->getClass() << ", but was declared in "
            << "rooms section to be of type " << className << " (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      // set Room's default title
      room->setTitle(getAttribute("name"));

      parseRoomProperties(room, 3);
      checkClosingTag(className);
   }

   /***************************************************************************/

   void Parser::parseRoomProperties(Room *room, int depth) {

      stringstream s;

      while (nextTag() && depth == getDepth()) {

         if (0 == getTagName().compare("title")) {
            room->setTitle(parseEntityTitle());
         }

         else if (0 == getTagName().compare("description")) {
            room->setLongDescription(parseEntityLongDescription());
         }

         else if (0 == getTagName().compare("short")) {
            room->setShortDescription(parseEntityShortDescription());
         }

         else if (isDirection(getTagName())) {
            string direction = getTagName();
            string connection = parseString();
            parseRoomConnection(direction, room, connection);
         }

         else if (0 == getTagName().compare("contains")) {
            parseRoomContains(room);
         }

         else if (0 == getTagName().compare("events")) {
            parseEvents(room->L, room->triggers, depth + 1);
         }

         else if (0 == getTagName().compare("meta")) {
            parseEntityMeta(room, depth + 1);
         }

         else if (0 == getTagName().compare("messages")) {
            Messages *m = parseMessages(depth + 1);
            room->setMessages(*m);
            delete m;
         }

         else {
            s << filename << ": invalid tag <" << getTagName() << "> in "
               << "room or class definition (line "
               << xmlTextReaderGetParserLineNumber(reader) << ")";
            throw s.str();
         }
      }
   }

   /***************************************************************************/

   void Parser::parseRoomConnection(string direction, Room *room, string connectTo) {

      stringstream s;

      Room *connectToRoom = rooms.get(connectTo);

      if (0 == connectToRoom) {
         s << filename << ": room '" << connectTo << "' does not exist (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      room->setConnection(direction, connectToRoom);
      checkClosingTag(direction);
   }

   /***************************************************************************/

   void Parser::parseRoomContains(Room *room) {

      stringstream s;

      while (nextTag() && 4 == getDepth()) {

         string tag = getTagName();

         if (0 == tag.compare("object") || 0 == tag.compare("creature")) {
            room->insertThing(parseRoomContainsThing(tag));
         }

         else {
            s << filename << ": invalid tag <" << getTagName() << "> in "
               << "rooms section (line "
               << xmlTextReaderGetParserLineNumber(reader) << ")";
            throw s.str();
         }
      }

      checkClosingTag("contains");
   }

   /***************************************************************************/

   Thing *Parser::parseRoomContainsThing(string tag) {

      stringstream s;

      string thingName = parseString();
      Thing *thing = things.get(thingName);

      if (0 == thing) {
         s << filename << ": Thing '" << tag << "' doesn't exist (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      checkClosingTag(tag);
      return thing;
   }

   /***************************************************************************/

   bool Parser::parseItemTakeable() {

      bool takeable = parseBool();
      checkClosingTag("takeable");
      return takeable;
   }

   /***************************************************************************/

   bool Parser::parseItemDroppable() {

      bool droppable = parseBool();
      checkClosingTag("droppable");
      return droppable;
   }

   /***************************************************************************/

   int Parser::parseObjectWeight() {

      int weight = parseInt();
      checkClosingTag("weight");
      return weight;
   }

   /***************************************************************************/

   bool Parser::parseItemWeapon() {

      bool isWeapon = parseBool();
      checkClosingTag("weapon");
      return isWeapon;
   }

   /***************************************************************************/

   int Parser::parseItemDamage() {

      int damage = parseInt();
      checkClosingTag("damage");
      return damage;
   }

   /***************************************************************************/

   bool Parser::parseCreatureCounterAttack() {

      bool counterAttack = parseBool();
      checkClosingTag("counterattack");
      return counterAttack;
   }

   /***************************************************************************/

   void Parser::parseCreatureAutoAttack(Creature *creature, int depth) {

      stringstream s;

      while (nextTag() && depth == getDepth()) {

         if (0 == getTagName().compare("enabled")) {
            creature->setAutoAttackEnabled(parseCreatureAutoAttackEnabled());
         }

         else if (0 == getTagName().compare("interval")) {
            creature->setAutoAttackInterval(parseCreatureAutoAttackInterval());
         }

         else if (0 == getTagName().compare("repeat")) {
            creature->setAutoAttackRepeat(parseCreatureAutoAttackRepeat());
         }

         else {
            s << filename << ": invalid tag <" << getTagName() << "> in "
               << "creature autoattack section (line "
               << xmlTextReaderGetParserLineNumber(reader) << ")";
            throw s.str();
         }
      }

      checkClosingTag("autoattack");
   }

   /***************************************************************************/

   bool Parser::parseCreatureAutoAttackEnabled() {

      bool enabled = parseBool();
      checkClosingTag("enabled");
      return enabled;
   }

   /***************************************************************************/

   int Parser::parseCreatureAutoAttackInterval() {

      int interval = parseInt();
      checkClosingTag("interval");
      return interval;
   }

   /***************************************************************************/

   bool Parser::parseCreatureAutoAttackRepeat() {

      bool repeat = parseBool();
      checkClosingTag("repeat");
      return repeat;
   }

   /***************************************************************************/

   void Parser::parseCreatureWandering(Creature *creature) {

      stringstream s;

      while (nextTag() && 4 == getDepth()) {

         string tag = getTagName();

         if (0 == tag.compare("enabled")) {
            creature->setWanderEnabled(parseCreatureWanderingEnabled());
         }

         else if (0 == tag.compare("interval")) {

            try {
               creature->setWanderInterval(parseCreatureWanderingInterval());
            }

            catch (char const *error) {
               s << filename << ": "
                  << error << " in creature wander interval setting (line "
                  << xmlTextReaderGetParserLineNumber(reader) << ")";
               throw s.str();
            }
         }

         else if (0 == tag.compare("wanderlust")) {

            try {
               creature->setWanderLust(parseCreatureWanderLust());
            }

            catch (char const *error) {
               s << filename << ": "
                  << error << " in creature wanderlust setting (line "
                  << xmlTextReaderGetParserLineNumber(reader) << ")";
               throw s.str();
            }
         }

         else {
            s << filename << ": invalid tag <" << getTagName() << "> in "
               << "creature wandering settings (line "
               << xmlTextReaderGetParserLineNumber(reader) << ")";
            throw s.str();
         }
      }

      checkClosingTag("wandering");
   }

   /***************************************************************************/

   bool Parser::parseCreatureWanderingEnabled() {

      bool enabled = parseBool();
      checkClosingTag("enabled");
      return enabled;
   }

   /***************************************************************************/

   int Parser::parseCreatureWanderingInterval() {

      int interval = parseInt();
      checkClosingTag("interval");
      return interval;
   }

   /***************************************************************************/

   double Parser::parseCreatureWanderLust() {

      double wanderlust = parseDouble();
      checkClosingTag("wanderlust");
      return wanderlust;
   }

   /***************************************************************************/

   enum entity::Creature::AllegianceType Parser::parseCreatureAllegiance() {

      stringstream s;

      string allegiance = strToLower(parseString());

      checkClosingTag("allegiance");

      if (0 == allegiance.compare("friend")) {
         return entity::Creature::FRIEND;
      }

      if (0 == allegiance.compare("neutral")) {
         return entity::Creature::NEUTRAL;
      }

      if (0 == allegiance.compare("enemy")) {
         return entity::Creature::ENEMY;
      }

      else {
         s << "Expecting one of 'friend', 'neutral' or 'enemy' in creature "
            << "allegiance setting (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }
   }

   /***************************************************************************/

   void Parser::parseBeingRespawn(Being *being, int depth) {

      stringstream s;

      while (nextTag() && depth == getDepth()) {

         if (0 == getTagName().compare("enabled")) {
            being->setRespawnEnabled(parseBeingRespawnEnabled());
         }

         else if (0 == getTagName().compare("interval")) {
            being->setRespawnInterval(parseBeingRespawnInterval());
         }

         else if (0 == getTagName().compare("lives")) {
            being->setRespawnLives(parseBeingRespawnLives());
         }

         else {
            s << filename << ": invalid tag <" << getTagName() << "> in "
               << "being respawn section (line "
               << xmlTextReaderGetParserLineNumber(reader) << ")";
            throw s.str();
         }
      }

      checkClosingTag("respawn");
   }

   /***************************************************************************/

   bool Parser::parseBeingRespawnEnabled() {

      bool enabled = parseBool();
      checkClosingTag("enabled");
      return enabled;
   }

   /***************************************************************************/

   int Parser::parseBeingRespawnInterval() {

      int interval = parseInt();
      checkClosingTag("interval");
      return interval;
   }

   /***************************************************************************/

   int Parser::parseBeingRespawnLives() {

      int lives = parseInt();
      checkClosingTag("lives");
      return lives;
   }

   /***************************************************************************/

   void Parser::parseBeingInventory(entity::Being *being, bool allowObjects) {

      stringstream s;

      while (nextTag() && 4 == getDepth()) {

         if (0 == getTagName().compare("weight")) {
            being->setInventoryWeight(parseBeingInventoryWeight());
         }

         else if (true == allowObjects && 0 == getTagName().compare("object")) {
            parseBeingInventoryObject(being);
         }

         else {
            s << filename << ": invalid tag <" << getTagName() << "> in "
               << "default player's inventory settings (line "
               << xmlTextReaderGetParserLineNumber(reader) << ")";
            throw s.str();
         }
      }

      checkClosingTag("inventory");
   }

   /***************************************************************************/

   void Parser::parseBeingInventoryObject(Being *being) {

      stringstream s;
      string objectName = parseString();

      Object *object = objects.get(objectName);

      if (0 == object) {
         s << filename << ": object '" << objectName << "' doesn't exist (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      else if (0 != object->getOwner()) {
         s << filename << ": object '" << objectName << "' is already owned by '"
            << object->getOwner()->getName() << "' (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      else if (0 != object->getLocation()) {
         s << filename << ": object '" << objectName << "' was already placed "
            << "in room '" << object->getLocation()->getName() << "' (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      being->insertIntoInventory(object, false);
      checkClosingTag("object");
   }

   /***************************************************************************/

   int Parser::parseBeingInventoryWeight() {

      int weight = parseInt();
      checkClosingTag("weight");
      return weight;
   }

   /***************************************************************************/

   void Parser::parseBeingAttributes(entity::Being *being) {

      stringstream s;

      while (nextTag() && 4 == getDepth()) {

         if (0 == getTagName().compare("strength")) {
            being->setAttribute("strength", parseBeingAttributesStrength());
         }

         else if (0 == getTagName().compare("dexterity")) {
            being->setAttribute("dexterity", parseBeingAttributesDexterity());
         }

         else if (0 == getTagName().compare("intelligence")) {
            being->setAttribute("intelligence", parseBeingAttributesIntelligence());
         }

         else {
            s << filename << ": invalid tag <" << getTagName() << "> in "
               << "default player's inventory settings (line "
               << xmlTextReaderGetParserLineNumber(reader) << ")";
            throw s.str();
         }
      }

      being->setAttributesInitialTotal();
      checkClosingTag("attributes");
   }

   /***************************************************************************/

   int Parser::parseBeingAttributesStrength() {

      int strength = parseInt();
      checkClosingTag("strength");
      return strength;
   }

   /***************************************************************************/

   int Parser::parseBeingAttributesDexterity() {

      int dexterity = parseInt();
      checkClosingTag("dexterity");
      return dexterity;
   }

   /***************************************************************************/

   int Parser::parseBeingAttributesIntelligence() {

      int intelligence = parseInt();
      checkClosingTag("intelligence");
      return intelligence;
   }

   /***************************************************************************/

   bool Parser::parseBeingAlive() {

      bool alive = parseBool();
      checkClosingTag("alive");
      return alive;
   }

   /***************************************************************************/

   int Parser::parseBeingHealth() {

      int health = parseInt();
      checkClosingTag("health");
      return health;
   }

   /***************************************************************************/

   int Parser::parseBeingMaxHealth() {

      int maxHealth = parseInt();
      checkClosingTag("maxHealth");
      return maxHealth;
   }

   /***************************************************************************/

   double Parser::parseBeingWoundRate() {

      double woundRate = parseDouble();
      checkClosingTag("woundrate");
      return woundRate;
   }

   /***************************************************************************/

   bool Parser::parseBeingAttackable() {

      bool attackable = parseBool();
      checkClosingTag("attackable");
      return attackable;
   }

   /***************************************************************************/

   int Parser::parseBeingDamageBareHands() {

      int damage = parseInt();
      checkClosingTag("damagebarehands");
      return damage;
   }

   /***************************************************************************/

   void Parser::parseThingAliases(Thing *thing, int depth) {

      stringstream s;

      while (nextTag() && depth == getDepth()) {

         if (0 == getTagName().compare("alias")) {
            thing->addAlias(parseThingAlias());
         }

         else {
            s << filename << ": invalid tag <" << getTagName() << "> in "
               << "aliases (line "
               << xmlTextReaderGetParserLineNumber(reader) << ")";
            throw s.str();
         }
      }

      checkClosingTag("aliases");
   }

   /***************************************************************************/

   string Parser::parseThingAlias() {

      string alias = parseString();
      checkClosingTag("alias");
      return trim(alias);
   }

   /***************************************************************************/

   string Parser::parseEntityTitle() {

      string title = parseString();
      checkClosingTag("title");
      return trim(title);
   }

   /***************************************************************************/

   string Parser::parseEntityLongDescription() {

      string longdesc = parseString();
      checkClosingTag("description");
      return trim(longdesc);
   }

   /***************************************************************************/

   string Parser::parseEntityShortDescription() {

      string shortdesc = parseString();
      checkClosingTag("short");
      return trim(shortdesc);
   }

   /***************************************************************************/

   bool Parser::parseBool() {

      stringstream s;

      s << filename << ": Expecting boolean 1 for true or 0 for false (line "
         << xmlTextReaderGetParserLineNumber(reader) << ")";

      try {
         return boost::lexical_cast<bool>(getNodeValue());
      }

      catch (boost::bad_lexical_cast) {
         throw s.str();
      }
   }

   /***************************************************************************/

   int Parser::parseInt() {

      stringstream s;

      s << filename << ": Expecting integer value (line "
         << xmlTextReaderGetParserLineNumber(reader) << ")";

      try {
         return boost::lexical_cast<int>(getNodeValue());
      }

      catch (boost::bad_lexical_cast) {
         throw s.str();
      }
   }

   /***************************************************************************/

   double Parser::parseDouble() {

      stringstream s;

      s << filename << ": Expecting floating point number (line "
         << xmlTextReaderGetParserLineNumber(reader) << ")";

      try {
         return boost::lexical_cast<double>(getNodeValue());
      }

      catch (boost::bad_lexical_cast) {
         throw s.str();
      }
   }

   /***************************************************************************/

   string Parser::parseString() {

      stringstream s;
      string value = getNodeValue();

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

      // TODO: skip comments (right now, we'll get an error!)

      if (xmlTextReaderRead(reader) < 0) {
         s << "Unknown error reading " << filename << " (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      else if (XML_ELEMENT_DECL != xmlTextReaderNodeType(reader) ||
      0 != strcmp(tag.c_str(), (const char *)xmlTextReaderConstName(reader))) {
         s << "missing closing </" << tag << "> (line " <<
            xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }
   }
}

