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

      // ignore any extra stuff that may exist in the XML file
      if (nextTag()) {
         cerr << filename << ": warning: ignoring contents after </game>" << endl;
      }
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
            parseScript(L, depth + 1);
         }

         else if (0 == getTagName().compare("event")) {
            parseEvent(triggers, depth + 1);
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

   void Parser::parseScript(LuaState *L, int depth) {

      // TODO
      cout << "<script>: " << getAttribute("src") << endl;
      checkClosingTag("script");
   }

   /***************************************************************************/

   void Parser::parseEvent(EventListener *triggers, int depth) {

      // TODO
      string name = getAttribute("name");
      string function = parseString();
      cout << "<event name=\"" << name << "\">" << function << "</event>" << endl;
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
            object->setWeight(parseItemWeight());
         }

         else if (0 == getTagName().compare("takeable")) {
            object->setTakeable(parseItemTakeable());
         }

         else if (0 == getTagName().compare("droppable")) {
            object->setDroppable(parseItemDroppable());
         }

         else if (0 == getTagName().compare("events")) {
            parseEvents(object->L, object->triggers, 4);
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

   int Parser::parseItemWeight() {

      int weight = parseInt();
      checkClosingTag("weight");
      return weight;
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
            parseBeingInventory(defaultPlayer);
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

   void Parser::parseBeingInventory(entity::Being *being) {

      stringstream s;

      while (nextTag() && 4 == getDepth()) {

         if (0 == getTagName().compare("weight")) {
            being->setInventoryWeight(parseBeingInventoryWeight());
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

