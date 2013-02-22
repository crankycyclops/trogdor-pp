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
      player = new entity::Player();
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

      // TODO: returns true if room name is start, and false 
      cout << "manifest room: " << getAttribute("name") << endl;
      checkClosingTag("room");
      return true;
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

      // TODO
      cout << "manifest object: " << getAttribute("name") << endl;
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

      // TODO
      cout << "manifest creature: " << getAttribute("name") << endl;
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

      m->set(getAttribute("name"), getNodeValue());
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
            // TODO: will need to pass pointer to struct
            parseBeingInventory();
         }

         else if (0 == getTagName().compare("attributes")) {
            // TODO: will need to pass pointer to struct
            parseBeingAttributes();
         }

         else if (0 == getTagName().compare("alive")) {
            // TODO: this returns bool, which we should then use
            parseBeingAlive();
         }

         else if (0 == getTagName().compare("health")) {
            // TODO: this returns int, which we should then use
            parseBeingHealth();
         }

         else if (0 == getTagName().compare("maxHealth")) {
            // TODO: this returns int, which we should then use
            parseBeingMaxHealth();
         }

         else if (0 == getTagName().compare("woundRate")) {
            // TODO: this returns int, which we should then use
            parseBeingWoundRate();
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

   void Parser::parseBeingInventory() {

      // TODO: will take as argument pointer to fields in struct/class
   }

   /***************************************************************************/

   void Parser::parseBeingAttributes() {

      // TODO: will return some sort of struct
   }

   /***************************************************************************/

   bool Parser::parseBeingAlive() {

      // TODO
   }

   /***************************************************************************/

   int Parser::parseBeingHealth() {

      // TODO
   }

   /***************************************************************************/

   int Parser::parseBeingMaxHealth() {

      // TODO
   }

   /***************************************************************************/

   double Parser::parseBeingWoundRate() {

      // TODO
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

