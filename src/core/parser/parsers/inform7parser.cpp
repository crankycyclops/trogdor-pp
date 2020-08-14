#include <memory>
#include <vector>

// TODO: remove this after testing
#include <iostream>

#include <trogdor/utility.h>
#include <trogdor/vocabulary.h>
#include <trogdor/exception/parseexception.h>

#include <trogdor/parser/parsers/inform7parser.h>

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

   void Inform7Parser::parseDescriptionStatement(std::string identifier) {

      Token t = lexer.peek();

      if (EQUALITY == t.type) {

         t = lexer.next();

         if (QUOTED_STRING != t.type) {
            throw ParseException(
               std::string("You said you were going to describe ") + identifier
               + " but never did (line " + std::to_string(t.lineno) + ')'
            );
         }

         else {
            lexer.push(t);
            parseDescription({identifier});
         }
      }

      else {
         throw ParseException(
            std::string("I can't find a verb that I know how to deal with. (line ")
            + std::to_string(t.lineno) + ')'
         );
      }
   }

   /**************************************************************************/

   void Inform7Parser::parseBibliographic() {

      Token t = lexer.next();

      std::string author;
      std::string title = t.value;

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
            throw ParseException(std::string("Initial bibliographic sentence has 'by' without author (line ") + std::to_string(t.lineno) + ')');
         }
      }

      else if (PHRASE_TERMINATOR != t.type) {
         throw ParseException(std::string("Initial bibliographic sentence can only be a title in double quotes, possibly followed with 'by' and the name of the author (line ") + std::to_string(t.lineno) + ')');
      }

      // TODO
      std::cout << "Stub: Title is \"" + title + "\" and author is \"" + (author.length() ? author : "(undefined)") << '\"' << std::endl;
   }

   /**************************************************************************/

   std::vector<std::string> Inform7Parser::parseIdentifiersList() {

      Token t;
      std::vector<std::string> identifiers;

      // Use this to make sure we only skip past articles if they really
      // function as articles by appearing before an identifier name
      bool startingNewNoun = true;

      do {

         std::string noun;

         // Skip past articles
         // TODO: need to keep track of whether or not an article was used; if
         // one was, then it's a proper noun. Otherwise, it's not. This effects
         // things like whether or not a room is visited when an object is in it.
         for (t = lexer.next(); startingNewNoun && ARTICLE == t.type; t = lexer.next());
         startingNewNoun = false;

         // Sometimes, we have a comma followed by and. In that case, skip over
         // the extraneous "and."
         if (AND == t.type) {
            t = lexer.next();
         }

         // Grab the next identifier
         for (; WORD == t.type || ARTICLE == t.type; t = lexer.next()) {
            noun += (noun.length() ? " " : "");
            noun += t.value;
         }

         if (noun.length()) {
            identifiers.push_back(noun);
            startingNewNoun = true;
         }

         else {
            throw ParseException(std::string("Sentence does not follow a valid syntax. Did you end with a dangling comma or \"and\"? (line ") + std::to_string(t.lineno) + ')');
         }
      } while (COMMA == t.type || AND == t.type);

      lexer.push(t);
      return identifiers;
   }

   /**************************************************************************/

   std::vector<Inform7Parser::ParsedProperty> Inform7Parser::parsePropertyList() {

      Token t = lexer.next();
      std::vector<Inform7Parser::ParsedProperty> propertyList;

      // Flags whether or not an adjective/property is preceded by "not"
      bool negated = false;

      while (
         properties.end() != properties.find(strToLower(t.value)) ||
         adjectives.end() != adjectives.find(strToLower(t.value)) ||
         0 == strToLower(t.value).compare("not")
      ) {

         // If a property is preceded by not, flag it so we can record the
         // negation when the property is parsed
         if (0 == strToLower(t.value).compare("not")) {
            negated = true;
            t = lexer.next();
            continue;
         }

         else if (adjectives.end() != adjectives.find(strToLower(t.value))) {
            throw ParseException(
               std::string("'") + strToLower(t.value)
               + "' is only an adjective that can be checked during runtime, not a property that can be set (line "
               + std::to_string(t.lineno) + ')'
            );
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

   void Inform7Parser::parseInClause(std::vector<std::string> subjects) {

      Token t;

      // Grab list of containers or places where the subject(s) should go (there
      // should only be one, and if there are more we'll report it as an error)
      std::vector<std::string> containersOrPlaces = parseIdentifiersList();

      if (!containersOrPlaces.size()) {
         throw ParseException(std::string("You said ") + vectorToStr(subjects) +
            " " + (subjects.size() > 1 ? "are" : "is") +
            " in a container or a place without saying what that container or " +
            "place is (line " + std::to_string(t.lineno) + ')');
      }

      else if (containersOrPlaces.size() > 1) {
         throw ParseException(std::string("You said ") + vectorToStr(subjects) +
            (subjects.size() > 1 ? " are" : " is") +
            " in " + vectorToStr(containersOrPlaces) + ", but " +
            (subjects.size() > 1 ? "they" : "it") +
            " can only be in one container or place at a time (line " +
            std::to_string(t.lineno) + ')');
      }

      else {
         // TODO
         std::cout << "parseInClause stub!" << std::endl << std::endl;
         std::cout << vectorToStr(subjects) << " " <<
            (subjects.size() > 1 ? "are" : "is") << " in " <<
            containersOrPlaces[0] << "." << std::endl << std::endl;
      }
   }

   /**************************************************************************/

   void Inform7Parser::parseOnClause(std::vector<std::string> subjects) {

      Token t;

      // Grab list of supports where the subject(s) should go (there should only
      // be one, and if there are more we'll report it as an error)
      std::vector<std::string> supporters = parseIdentifiersList();

      if (!supporters.size()) {
         throw ParseException(std::string("You said ") + vectorToStr(subjects) +
            " " + (subjects.size() > 1 ? "are" : "is") +
            " on a supporter without saying what that supporter is (line " +
            std::to_string(t.lineno) + ')');
      }

      else if (supporters.size() > 1) {
         throw ParseException(std::string("You said ") + vectorToStr(subjects) +
            (subjects.size() > 1 ? " are" : " is") +
            " on " + vectorToStr(supporters) + ", but " +
            (subjects.size() > 1 ? "they" : "it") +
            " can only be on one supporter at a time (line " +
            std::to_string(t.lineno) + ')');
      }

      else {
         // TODO
         std::cout << "parseOnClause stub!" << std::endl << std::endl;
         std::cout << vectorToStr(subjects) << " " <<
            (subjects.size() > 1 ? "are" : "is") << " on " <<
            supporters[0] << "." << std::endl << std::endl;
      }
   }

   /**************************************************************************/

   void Inform7Parser::parseLocationClause() {

      Token t;

      // TODO skip to end of phrase until we implement this
      std::cout << std::endl << "parseLocationClause stub!" << std::endl;
      for (t = lexer.peek(); PHRASE_TERMINATOR != t.type; t = lexer.next());
      lexer.push(t);
   }

   /**************************************************************************/

   void Inform7Parser::parseDescription(std::vector<std::string> identifiers) {

      Token t = lexer.next();

      // A description of the thing being described was included
      if (QUOTED_STRING == t.type) {

         std::string description = t.value;

         if (identifiers.size() > 1) {
         throw ParseException(std::string("You wrote '") + description +
            "' (line " + std::to_string(t.lineno) + "): but I don't know if " +
            "you're trying to describe " + vectorToStr(identifiers, "or") +
            ".");
         }

         t = lexer.next();

         if (PHRASE_TERMINATOR != t.type && SOURCE_EOF != t.type) {

            std::string combined = "\"" + description + "\"";

            for (; PHRASE_TERMINATOR != t.type && SOURCE_EOF != t.type; t = lexer.next()) {

               if (COMMA != t.type && COLON != t.type && SEMICOLON != t.type) {
                  combined += " ";
               }

               combined += t.value;
            }

            throw ParseException(std::string("You wrote '") + combined +
               "' (line " + std::to_string(t.lineno) + "): but it looks as if " +
               "perhaps you did not intend that to read as a single " +
               "sentence, and possibly the text in quotes was supposed to " +
               "stand as as a sentence on its own? (The convention is " +
               "that if text ends in a full stop, exclamation or question " +
               "mark, perhaps with a close bracket or quotation mark " +
               "involved as well, then that punctuation mark also closes " +
               "the sentence to which the text belongs: but otherwise the " +
               "words following the quoted text are considered part of " +
               "the same sentence.");
         }

         else {
            // TODO
            std::cout << "parseDescription stub!" << std::endl;
            std::cout << "Description of " + identifiers[0] + ": " << description
               << std::endl << std::endl;
         }
      } else {
         lexer.push(t);
      }
   }

   /**************************************************************************/

   void Inform7Parser::parseDefinition(std::vector<std::string> identifiers,
   std::vector<Inform7Parser::ParsedProperty> propertyList) {

      Token t = lexer.peek();

      // TODO
      std::cout << std::endl << "parseDefinition stub!" << std::endl << std::endl;

      std::cout << "Identifiers: " << std::endl;
      for (auto i = identifiers.begin(); i != identifiers.end(); i++) {
         std::cout << *i << std::endl;
      }

      std::cout << std::endl;

      if (propertyList.size()) {
         std::cout << "Properties: " << std::endl;
         for (auto i = propertyList.begin(); i != propertyList.end(); i++) {
            std::cout << i->value + (i->negated ? " (negated)" : "") << std::endl;
         }
      } else {
         std::cout << "(No properties.)" << std::endl;
      }

      if (
         classes.end() != classes.find(strToLower(t.value)) ||
         classPlurals.end() != classPlurals.find(strToLower(t.value))
      ) {      

         std::string className = classPlurals.end() != classPlurals.find(strToLower(t.value)) ?
            classPlurals[strToLower(t.value)] : strToLower(t.value);

         // TODO
         std::cout << std::endl << "Identifiers are of type '" << className << "'" << std::endl;

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

      lexer.next();
   }

   /**************************************************************************/

   void Inform7Parser::parsePropertyAssignment(std::vector<std::string> identifiers,
   std::vector<Inform7Parser::ParsedProperty> propertyList) {

      // TODO
      std::cout << std::endl << "parsePropertyAssignment stub!" << std::endl << std::endl;

      std::cout << "Identifiers: " << std::endl;
      for (auto i = identifiers.begin(); i != identifiers.end(); i++) {
         std::cout << *i << std::endl;
      }

      std::cout << std::endl << "Properties: " << std::endl;
      for (auto i = propertyList.begin(); i != propertyList.end(); i++) {
         std::cout << i->value + (i->negated ? " (negated)" : "") << std::endl;
      }
   }

   /**************************************************************************/

   void Inform7Parser::parsePlacement(std::vector<std::string> subjects) {

      Token t = lexer.peek();

      if (0 == strToLower(t.value).compare("in")) {
         parseInClause(subjects);
      }

      else {
         parseOnClause(subjects);
      }

      lexer.next();
   }

   /**************************************************************************/

   void Inform7Parser::parseEquality(std::vector<std::string> identifiers) {

      Token t;

      // Skip past any articles that might be present
      for (t = lexer.next(); ARTICLE == t.type; t = lexer.next());
      lexer.push(t);

      if (
         properties.end() != properties.find(strToLower(t.value)) ||
         adjectives.end() != adjectives.find(strToLower(t.value)) ||
         0 == strToLower(t.value).compare("not")
      ) {

         std::vector<Inform7Parser::ParsedProperty> propertyList;

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
            throw ParseException(std::string("Your sentence on line ") + std::to_string(t.lineno) + " seems to refer to something whose name begins with a comma, which is forbidden.");
         }

         else {
            throw ParseException(std::string("Illegal character on line ") + std::to_string(t.lineno) + ".");
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
         throw ParseException(std::string("'") + t.value + "' is not a known kind of thing or property of a thing (line " + std::to_string(t.lineno) + ')');
      }

      else if (QUOTED_STRING == t.type) {
         throw ParseException(std::string("The sentence on line ") + std::to_string(t.lineno) + " appears to say that one or more things are equal to a value, but that makes no sense.");
      }

      else {
         throw ParseException(std::string("I can't find a verb that I know how to deal with. (line ") + std::to_string(t.lineno) + ')');
      }

      t = lexer.peek();

      if (PHRASE_TERMINATOR == t.type) {
         parseDescription(identifiers);
      } else {
         lexer.push(t);
      }
   }

   /**************************************************************************/

   void Inform7Parser::parsePhrase() {

      Token t;

      // We're going to break away from strict LL parsing for a moment, because
      // otherwise there would be too much lookahead
      std::vector<std::string> identifiers = parseIdentifiersList();
      t = lexer.next();

      // This is a bit hacky, but it works and results in minimal changes to my
      // code. If relying on parseIdentifiersList() ever causes me problems in
      // the future, I'll revisit this.
      size_t descriptionOfIndex;
      if (1 == identifiers.size() && 0 == (descriptionOfIndex = identifiers[0].find("description of "))) {
         parseDescriptionStatement(identifiers[0].substr(15));
      }

      else if (!identifiers.size()) {
         throw ParseException(std::string("You've used a verb ('" + t.value + "') without a subject (line ") + std::to_string(t.lineno) + ')');
      }

      // We're parsing an expression with an "is" or "are" verb
      else if (EQUALITY == t.type) {
         parseEquality(identifiers);
      }

      else {
         throw ParseException(std::string("I can't find a verb that I know how to deal with. (line ") + std::to_string(t.lineno) + ')');
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

   void Inform7Parser::parse(std::string filename) {

      lexer.open(filename);
      parseProgram();

      // TODO: start using the AST (undefined so far) to populate the game
   }
}
