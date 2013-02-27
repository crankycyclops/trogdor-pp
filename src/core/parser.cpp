#include "include/parser.h"

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
      defaultPlayer = new entity::Player(game, "default");
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

      // the manifest ALWAYS comes first!
      nextTag();
      if (0 != getTagName().compare("manifest")) {
         s << filename << ": expected <manifest> (line " <<
            xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      parseManifest();

      // parse the remaining sections
      while (nextTag() && 1 == getDepth()) {

         if (0 == getTagName().compare("events")) {
            parseEvents(gameL, eventListener, 2);
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

         else {
            s << filename << ": invalid section <" << getTagName() <<
               "> (line " << xmlTextReaderGetParserLineNumber(reader) << ")";
            throw s.str();
         }
      }

      checkClosingTag("game");
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

   bool Parser::parseManifestRoom() {

      string name = getAttribute("name");

      if (entities.isset(name)) {
         stringstream s;
         s << "Cannot define room with name '" << name << ": an entity with " <<
            "that name already exists";
         throw s.str();
      }

      Room *room = new Room(game, name);

      entities.set(name, room);
      places.set(name, room);
      rooms.set(name, room);

      checkClosingTag("room");
      return 0 == name.compare("start") ? true : false;
   }

   /***************************************************************************/

   void Parser::parseManifestObjects() {

      stringstream s;

      while (nextTag() && 3 == getDepth()) {

         if (0 == getTagName().compare("object")) {
            parseManifestObject();
         }

         else {
            s << filename << ": invalid tag <" << getTagName() << "> in "
               << "objects section of the manifest (line "
               << xmlTextReaderGetParserLineNumber(reader) << ")";
            throw s.str();
         }
      }

      checkClosingTag("objects");
   }

   /***************************************************************************/

   void Parser::parseManifestObject() {

      string name = getAttribute("name");

      if (entities.isset(name)) {
         stringstream s;
         s << "Cannot define object with name '" << name << ": an entity with "
            << "that name already exists";
         throw s.str();
      }

      Object *object = new Object(game, name);

      entities.set(name, object);
      things.set(name, object);
      items.set(name, object);
      objects.set(name, object);

      checkClosingTag("object");
   }

   /***************************************************************************/

   void Parser::parseManifestCreatures() {

      stringstream s;

      while (nextTag() && 3 == getDepth()) {

         if (0 == getTagName().compare("creature")) {
            parseManifestCreature();
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

   void Parser::parseManifestCreature() {

      string name = getAttribute("name");

      if (entities.isset(name)) {
         stringstream s;
         s << "Cannot define creature with name '" << name << ": an entity "
            << "with that name already exists";
         throw s.str();
      }

      Creature *creature = new Creature(game, name);

      entities.set(name, creature);
      things.set(name, creature);
      beings.set(name, creature);
      creatures.set(name, creature);

      checkClosingTag("creature");
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

      L->loadScriptFromFile(getAttribute("src"));
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

   void Parser::parseObject() {

      stringstream s;

      Object *object = objects.get(getAttribute("name"));

      if (0 == object) {
         s << filename << ": object '" << getAttribute("name") << "' was not "
            << "declared in the manifest (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      while (nextTag() && 3 == getDepth()) {

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
            parseEvents(object->L, object->triggers, 4);
         }

         else if (0 == getTagName().compare("aliases")) {
            parseThingAliases(object);
         }

         else if (0 == getTagName().compare("messages")) {
            Messages *m = parseMessages(4);
            object->setMessages(*m);
            delete m;
         }

         else {
            s << filename << ": invalid tag <" << getTagName() << "> in "
               << "object definition (line "
               << xmlTextReaderGetParserLineNumber(reader) << ")";
            throw s.str();
         }
      }

      checkClosingTag("object");
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

         if (0 == getTagName().compare("inventory")) {
            parseBeingInventory(defaultPlayer, false);
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

   void Parser::parseCreature() {

      stringstream s;

      Creature *creature = creatures.get(getAttribute("name"));

      if (0 == creature) {
         s << filename << ": creature '" << getAttribute("name") << "' was not "
            << "declared in the manifest (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      while (nextTag() && 3 == getDepth()) {

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

         else if (0 == getTagName().compare("counterattack")) {
            // TODO
            cout << "Counterattack stub!" << endl;
            parseBool();
            checkClosingTag("counterattack");
         }

         else if (0 == getTagName().compare("allegiance")) {
            creature->setAllegiance(parseCreatureAllegiance());
         }

         else if (0 == getTagName().compare("inventory")) {
            parseBeingInventory(creature, true);
         }

         else if (0 == getTagName().compare("attributes")) {
            parseBeingAttributes(creature);
         }

         else if (0 == getTagName().compare("events")) {
            parseEvents(creature->L, creature->triggers, 4);
         }

         else if (0 == getTagName().compare("aliases")) {
            parseThingAliases(creature);
         }

         else if (0 == getTagName().compare("messages")) {
            Messages *m = parseMessages(4);
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

      checkClosingTag("creature");
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

   void Parser::parseRoom() {

      stringstream s;

      Room *room = rooms.get(getAttribute("name"));

      if (0 == room) {
         s << filename << ": room '" << getAttribute("name") << "' was not "
            << "declared in the manifest (line "
            << xmlTextReaderGetParserLineNumber(reader) << ")";
         throw s.str();
      }

      while (nextTag() && 3 == getDepth()) {

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
            parseEvents(room->L, room->triggers, 4);
         }

         else if (0 == getTagName().compare("messages")) {
            Messages *m = parseMessages(4);
            room->setMessages(*m);
            delete m;
         }

         else {
            s << filename << ": invalid tag <" << getTagName() << "> in "
               << "room definition (line "
               << xmlTextReaderGetParserLineNumber(reader) << ")";
            throw s.str();
         }
      }

      checkClosingTag("room");
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
            being->setStrength(parseBeingAttributesStrength());
         }

         else if (0 == getTagName().compare("dexterity")) {
            being->setDexterity(parseBeingAttributesDexterity());
         }

         else if (0 == getTagName().compare("intelligence")) {
            being->setIntelligence(parseBeingAttributesIntelligence());
         }

         else {
            s << filename << ": invalid tag <" << getTagName() << "> in "
               << "default player's inventory settings (line "
               << xmlTextReaderGetParserLineNumber(reader) << ")";
            throw s.str();
         }
      }

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

   void Parser::parseThingAliases(Thing *thing) {

      stringstream s;

      while (nextTag() && 4 == getDepth()) {

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
      return alias;
   }

   /***************************************************************************/

   string Parser::parseEntityTitle() {

      string title = parseString();
      checkClosingTag("title");
      return title;
   }

   /***************************************************************************/

   string Parser::parseEntityLongDescription() {

      string longdesc = parseString();
      checkClosingTag("description");
      return longdesc;
   }

   /***************************************************************************/

   string Parser::parseEntityShortDescription() {

      string shortdesc = parseString();
      checkClosingTag("short");
      return shortdesc;
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

