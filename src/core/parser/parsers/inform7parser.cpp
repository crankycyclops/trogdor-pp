#include <memory>
#include <vector>

#include "../../include/utility.h"
#include "../../include/vocabulary.h"
#include "../../include/exception/parseexception.h"

#include "../../include/parser/parsers/inform7parser.h"

namespace trogdor {


   Inform7Parser::Inform7Parser(std::unique_ptr<Instantiator> i,
   const Vocabulary &v): Parser(std::move(i), v), lexer(directions, classes,
   properties, adjectives) {

      // Built-in directions that Inform 7 recognizes by default. List can be
      // extended later. This structure maps directions to their opposites.
      insertDirection("up", "down");
      insertDirection("inside", "outside");
      insertDirection("north", "south");
      insertDirection("east", "west");
      insertDirection("northwest", "southeast");
      insertDirection("northeast", "southwest");

      // Built-in classes that Inform 7 recognizes by default.
      insertClass("object");
      insertClass("direction");
      insertClass("room");
      insertClass("region");
      insertClass("thing");
      insertClass("door");
      insertClass("container");
      insertClass("vehicle");
      insertClass("player's holdall");
      insertClass("supporter");
      insertClass("backdrop");
      insertClass("device");
      insertClass("person");
      insertClass("man");
      insertClass("woman");
      insertClass("animal");

      // Built-in properties that Inform 7 recognizes for certain classes by
      // default.
      insertProperty("touchable");
      insertProperty("untouchable");
      insertProperty("lighted");
      insertProperty("dark");
      insertProperty("open");
      insertProperty("closed");

      // These properties can only be set on things
      insertProperty("lit");
      insertProperty("unlit");
      insertProperty("transparent");
      insertProperty("opaque");
      insertProperty("fixed in place");
      insertProperty("portable");
      insertProperty("openable");
      insertProperty("unopenable");
      insertProperty("enterable");
      insertProperty("pushable between rooms");
      insertProperty("wearable");
      insertProperty("edible");
      insertProperty("inedible");
      insertProperty("described");
      insertProperty("undescribed");

      // These properties can only be set on doors
      insertProperty("locked");
      insertProperty("unlocked");
      insertProperty("lockable");
      insertProperty("unlockable");

      // These properties can only be set on people or animals
      insertProperty("male");
      insertProperty("female");
      insertProperty("neuter"); // as opposed to male or female

      // These properties can only be set on devices
      insertProperty("switched on");
      insertProperty("switched off");

      // Built-in adjectives that Inform 7 recognizes by default. List can be
      // extended later.
      insertAdjective("adjacent");
      insertAdjective("visible");
      insertAdjective("invisible");
      insertAdjective("visited");
      insertAdjective("empty");
      insertAdjective("non-empty");
      insertAdjective("carried");

      // These special adjectives are for numerical values only
      insertAdjective("positive");
      insertAdjective("negative");
      insertAdjective("even");
      insertAdjective("odd");
   }

   /**************************************************************************/

   Inform7Parser::~Inform7Parser() {

      // TODO
   }

   /**************************************************************************/

   void Inform7Parser::parseBibliographic() {

      Token t = lexer.next();

      string author;
      string title = t.value;

      t = lexer.next();

      if (0 == strToLower(t.value).compare("by")) {

         for (t = lexer.next(); (
            t.type != SOURCE_EOF &&
            t.type != PHRASE_TERMINATOR &&
            t.type != COLON &&
            t.type != SEMICOLON
         ); t = lexer.next()) {

            if (author.length()) {
               author += " ";
            }

            author += t.value;
         }

         if (!author.length()) {
            throw ParseException(string("Initial bibliographic sentence has 'by' without author (line ") + to_string(t.lineno) + ')');
         }
      }

      else if (PHRASE_TERMINATOR != t.type) {
         throw ParseException(string("Initial bibliographic sentence can only be a title in double quotes, possibly followed with 'by' and the name of the author (line ") + to_string(t.lineno) + ')');
      }

      // TODO
      cout << "Stub: Title is \"" + title + "\" and author is \"" + (author.length() ? author : "(undefined)") << '\"' << endl;
   }

   /**************************************************************************/

   vector<string> Inform7Parser::parseIdentifiersList() {

      Token t;
      vector<string> identifiers;

      do {

         string noun;

         // Skip past articles
         // TODO: need to keep track of whether or not an article was used; if
         // one was, then it's a proper noun. Otherwise, it's not. This effects
         // things like whether or not a room is visited when an object is in it.
         for (t = lexer.next(); ARTICLE == t.type; t = lexer.next());

         // Sometimes, we have a comma followed by and. In that case, skip over
         // the extraneous "and."
         if (AND == t.type) {
            t = lexer.next();
         }

         // Grab the next identifier
         for (; WORD == t.type; t = lexer.next()) {
            noun += (noun.length() ? " " : "");
            noun += t.value;
         }

         if (noun.length()) {
            identifiers.push_back(noun);
         }
      } while (COMMA == t.type || AND == t.type);

      lexer.push(t);
      return identifiers;
   }

   /**************************************************************************/

   vector<Inform7Parser::ParsedProperty> Inform7Parser::parsePropertyList() {

      Token t = lexer.next();
      vector<Inform7Parser::ParsedProperty> propertyList;

      // Flags whether or not an adjective is preceded by "not"
      bool negated = false;

      while (
         properties.end() != properties.find(strToLower(t.value)) ||
         0 == strToLower(t.value).compare("not")
      ) {

         // If a property is preceded by not, flag it so we can record the
         // negation when the property is parsed
         if (0 == strToLower(t.value).compare("not")) {
            negated = true;
            t = lexer.next();
            continue;
         }

         propertyList.push_back({t.value, negated});
         t = lexer.next();

         // Properties might be delimited by a comma and/or "and" rather than
         // just a space
         if (COMMA == t.type) {
            t = lexer.next();
         }

         if (AND == t.type) {
            t = lexer.next();
         }

         // Reset the negated flag for the next property
         if (0 != strToLower(t.value).compare("not")) {
            negated = false;
         }
      }

      lexer.push(t);
      return propertyList;
   }

   /**************************************************************************/

   void Inform7Parser::parseInClause(vector<string> subjects) {

      Token t;

      // Grab list of containers or places where the subject(s) should go (there
      // should only be one, and if there are more we'll report it as an error)
      vector<string> containersOrPlaces = parseIdentifiersList();

      if (!containersOrPlaces.size()) {
         throw ParseException(string("You said ") + vectorToStr(subjects) +
            " " + (subjects.size() > 1 ? "are" : "is") +
            " in a container or a place without saying what that container or " +
            "place is (line " + to_string(t.lineno) + ')');
      }

      else if (containersOrPlaces.size() > 1) {
         throw ParseException(string("You said ") + vectorToStr(subjects) +
            (subjects.size() > 1 ? " are" : " is") +
            " in " + vectorToStr(containersOrPlaces) + ", but " +
            (subjects.size() > 1 ? "they" : "it") +
            " can only be in one container or place at a time (line " +
            to_string(t.lineno) + ')');
      }

      else {
         // TODO
         cout << "parseInClause stub!" << endl << endl;
         cout << vectorToStr(subjects) << " " <<
            (subjects.size() > 1 ? "are" : "is") << " in " <<
            containersOrPlaces[0] << "." << endl << endl;
      }
   }

   /**************************************************************************/

   void Inform7Parser::parseOnClause(vector<string> subjects) {

      Token t;

      // Grab list of supports where the subject(s) should go (there should only
      // be one, and if there are more we'll report it as an error)
      vector<string> supporters = parseIdentifiersList();

      if (!supporters.size()) {
         throw ParseException(string("You said ") + vectorToStr(subjects) +
            " " + (subjects.size() > 1 ? "are" : "is") +
            " on a supporter without saying what that supporter is (line " +
            to_string(t.lineno) + ')');
      }

      else if (supporters.size() > 1) {
         throw ParseException(string("You said ") + vectorToStr(subjects) +
            (subjects.size() > 1 ? " are" : " is") +
            " on " + vectorToStr(supporters) + ", but " +
            (subjects.size() > 1 ? "they" : "it") +
            " can only be on one supporter at a time (line " +
            to_string(t.lineno) + ')');
      }

      else {
         // TODO
         cout << "parseOnClause stub!" << endl << endl;
         cout << vectorToStr(subjects) << " " <<
            (subjects.size() > 1 ? "are" : "is") << " on " <<
            supporters[0] << "." << endl << endl;
      }
   }

   /**************************************************************************/

   void Inform7Parser::parseLocationClause() {

      Token t;

      // TODO skip to end of phrase until we implement this
      cout << endl << "parseLocationClause stub!" << endl;
      for (t = lexer.peek(); PHRASE_TERMINATOR != t.type; t = lexer.next());
      lexer.push(t);
   }

   /**************************************************************************/

   void Inform7Parser::parseDefinition(vector<string> identifiers,
   vector<Inform7Parser::ParsedProperty> propertyList) {

      Token t = lexer.peek();

      // TODO
      cout << endl << "parseDefinition stub!" << endl << endl;

      cout << "Identifiers: " << endl;
      for (auto i = identifiers.begin(); i != identifiers.end(); i++) {
         cout << *i << endl;
      }

      cout << endl;

      if (propertyList.size()) {
         cout << "Properties: " << endl;
         for (auto i = propertyList.begin(); i != propertyList.end(); i++) {
            cout << i->value + (i->negated ? " (negated)" : "") << endl;
         }
      } else {
         cout << "(No properties.)" << endl;
      }

      if (
         classes.end() != classes.find(strToLower(t.value)) ||
         classPlurals.end() != classPlurals.find(strToLower(t.value))
      ) {      

         string className = classPlurals.end() != classPlurals.find(strToLower(t.value)) ?
            classPlurals[strToLower(t.value)] : strToLower(t.value);

         // TODO
         cout << endl << "Identifiers are of type '" << className << "'" << endl;

         t = lexer.next();

         if (0 == strToLower(t.value).compare("in")) {
            parseInClause(identifiers);
         }

         else if (0 == strToLower(t.value).compare("on")) {
            parseOnClause(identifiers);
         }

         else {
            lexer.push(t);
         }
      }

      else if (directions.end() != directions.find(strToLower(t.value))) {
         parseLocationClause();
      }

      t = lexer.next();

      if (PHRASE_TERMINATOR == t.type) {

         t = lexer.next();

         // TODO: we're skipping past the description part of the definition (if
         // it exists) for now until we actually implement this
         if (QUOTED_STRING == t.type) {
            for (t = lexer.next(); PHRASE_TERMINATOR != t.type; t = lexer.next());
         } else {
            lexer.push(t);
         }
      }
   }

   /**************************************************************************/

   void Inform7Parser::parsePropertyAssignment(vector<string> identifiers,
   vector<Inform7Parser::ParsedProperty> propertyList) {

      // TODO
      cout << endl << "parsePropertyAssignment stub!" << endl << endl;

      cout << "Identifiers: " << endl;
      for (auto i = identifiers.begin(); i != identifiers.end(); i++) {
         cout << *i << endl;
      }

      cout << endl << "Properties: " << endl;
      for (auto i = propertyList.begin(); i != propertyList.end(); i++) {
         cout << i->value + (i->negated ? " (negated)" : "") << endl;
      }
   }

   /**************************************************************************/

   void Inform7Parser::parsePlacement(vector<string> subjects) {

      Token t;

      if (0 == strToLower(t.value).compare("in")) {
         parseInClause(subjects);
      }

      else {
         parseOnClause(subjects);
      }

      lexer.next();
   }

   /**************************************************************************/

   void Inform7Parser::parseEquality(vector<string> identifiers) {

      Token t;

      // Skip past any articles that might be present
      for (t = lexer.next(); ARTICLE == t.type; t = lexer.next());
      lexer.push(t);

      if (
         properties.end() != properties.find(strToLower(t.value)) ||
         0 == strToLower(t.value).compare("not")
      ) {

         vector<Inform7Parser::ParsedProperty> propertyList;

         // If we have a list of properties, parse them. We're breaking away
         // from a standard recursive descent in order to avoid too much
         // lookahead.
         propertyList = parsePropertyList();

         t = lexer.next();

         if (PHRASE_TERMINATOR == t.type) {
            parsePropertyAssignment(identifiers, propertyList);
         }

         else if (WORD == t.type) {
            parseDefinition(identifiers, propertyList);
         }

         else if (COMMA == t.type) {
            throw ParseException(string("Your sentence on line ") + to_string(t.lineno) + " seems to refer to something whose name begins with a comma, which is forbidden.");
         }

         else {
            throw ParseException(string("Illegal character on line ") + to_string(t.lineno) + ".");
         }
      }

      // We can define something in one of two ways. Either we can explicitly
      // say that one or more things are of a certain type, or we can
      // instantiate a room implicitly by saying that the name of a room is in a
      // certain direction from another already defined room.
      else if (
         classes.end() != classes.find(strToLower(t.value)) ||
         classPlurals.end() != classPlurals.find(strToLower(t.value)) ||
         directions.end() != directions.find(strToLower(t.value))
      ) {
         lexer.next();
         parseDefinition(identifiers);
      }

      else if (0 == strToLower(t.value).compare("in") ||
      0 == strToLower(t.value).compare("on")) {
         lexer.next();
         parsePlacement(identifiers);
      }

      else if (WORD == t.type || EQUALITY == t.type || AND == t.type) {
         throw ParseException(string("'") + t.value + "' is not a known kind of thing or property of a thing (line " + to_string(t.lineno) + ')');
      }

      else if (QUOTED_STRING == t.type) {
         throw ParseException(string("The sentence on line ") + to_string(t.lineno) + "appears to say that one or more things are equal to a value, but that makes no sense.");
      }

      else {
         throw ParseException(string("I can't find a verb that I know how to deal with. (line ") + to_string(t.lineno) + ')');
      }
   }

   /**************************************************************************/

   void Inform7Parser::parsePhrase() {

      Token t;

      // We're going to break away from strict LL parsing for a moment, because
      // otherwise there would be too much lookahead
      vector<string> identifiers = parseIdentifiersList();
      t = lexer.next();

      if (!identifiers.size()) {
         throw ParseException(string("You've used a verb ('" + t.value + "') without a subject (line ") + to_string(t.lineno) + ')');
      }

      // We're parsing an expression with an "is" or "are" verb
      if (EQUALITY == t.type) {
         parseEquality(identifiers);
      }

      else {
         throw ParseException(string("I can't find a verb that I know how to deal with. (line ") + to_string(t.lineno) + ')');
      }
   }

   /**************************************************************************/

   void Inform7Parser::parseProgram() {

      Token t = lexer.next();
      lexer.push(t);

      if (QUOTED_STRING == t.type) {
         parseBibliographic();
      }

      for (t = lexer.next(); SOURCE_EOF != t.type; t = lexer.next()) {
         lexer.push(t);
         parsePhrase();
      }
   }

   /**************************************************************************/

   void Inform7Parser::parse(string filename) {

      lexer.open(filename);
      parseProgram();

      // TODO: start using the AST (undefined so far) to populate the game
   }
}

