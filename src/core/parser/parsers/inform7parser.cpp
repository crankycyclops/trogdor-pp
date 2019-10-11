#include <memory>
#include <vector>

#include "../../include/utility.h"
#include "../../include/vocabulary.h"
#include "../../include/exception/parseexception.h"

#include "../../include/parser/parsers/inform7parser.h"

namespace trogdor {


   Inform7Parser::Inform7Parser(std::unique_ptr<Instantiator> i,
   const Vocabulary &v): Parser(std::move(i), v), lexer(directions, classes, adjectives) {

      // Built-in directions that Inform 7 recognizes by default. List can be
      // extended later. This structure maps directions to their opposites.
      directions["up"] = "down";
      directions["down"] = "up";
      directions["inside"] = "outside";
      directions["outside"] = "inside";
      directions["north"] = "south";
      directions["south"] = "north";
      directions["east"] = "west";
      directions["west"] = "east";
      directions["northwest"] = "southeast";
      directions["northeast"] = "southwest";
      directions["southwest"] = "northeast";
      directions["southeast"] = "northwest";

      // Built-in classes that Inform 7 recognizes by default. List can be
      // extended later.
      classes.insert("object");
      classes.insert("direction");
      classes.insert("room");
      classes.insert("region");
      classes.insert("thing");
      classes.insert("door");
      classes.insert("container");
      classes.insert("vehicle");
      classes.insert("player's holdall");
      classes.insert("supporter");
      classes.insert("backdrop");
      classes.insert("device");
      classes.insert("person");
      classes.insert("man");
      classes.insert("woman");
      classes.insert("animal");

      // Build a lookup table mapping each class to its plural equivalent. This
      // table should be updated whenever a class is added, either one that's
      // built-in or one that's defined later in Inform 7 source code.
      for (auto i = classes.begin(); classes.end() != i; i++) {
         classPlurals[*i] = language.pluralizeNoun(*i);
      }

      // Built-in adjectives that Inform 7 recognizes by default. List can be
      // extended later.
      adjectives.insert("adjacent");
      adjectives.insert("visible");
      adjectives.insert("invisible");
      adjectives.insert("visited");
      adjectives.insert("touchable");
      adjectives.insert("untouchable");
      adjectives.insert("lighted");
      adjectives.insert("dark");
      adjectives.insert("open");
      adjectives.insert("closed");
      adjectives.insert("empty");
      adjectives.insert("non-empty");

      // These adjectives (properties) can only be set on things
      adjectives.insert("lit");
      adjectives.insert("unlit");
      adjectives.insert("transparent");
      adjectives.insert("opaque");
      adjectives.insert("fixed in place");
      adjectives.insert("portable");
      adjectives.insert("openable");
      adjectives.insert("unopenable");
      adjectives.insert("enterable");
      adjectives.insert("pushable between rooms");
      adjectives.insert("wearable");
      adjectives.insert("edible");
      adjectives.insert("inedible");
      adjectives.insert("described");
      adjectives.insert("undescribed");

      // These adjectives can only be set on people or animals
      adjectives.insert("male");
      adjectives.insert("female");
      adjectives.insert("neuter"); // as opposed to male or female

      // These adjectives can only be set on doors
      adjectives.insert("locked");
      adjectives.insert("unlocked");
      adjectives.insert("lockable");
      adjectives.insert("unlockable");

      // These adjectives apply only to devices
      adjectives.insert("switched on");
      adjectives.insert("switched off");

      // These special adjectives are for numerical values only
      adjectives.insert("positive");
      adjectives.insert("negative");
      adjectives.insert("even");
      adjectives.insert("odd");
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

   vector<string> Inform7Parser::parseAdjectivesList() {

      Token t = lexer.next();
      vector<string> adjectiveList;

      while (adjectives.end() != adjectives.find(strToLower(t.value))) {

         adjectiveList.push_back(t.value);

         t = lexer.next();

         // Adjectives might be delimited by a comma and/or "and" rather than
         // just a space
         if (COMMA == t.type) {
            t = lexer.next();
         }

         if (AND == t.type) {
            t = lexer.next();
         }
      }

      lexer.push(t);
      return adjectiveList;
   }

   /**************************************************************************/

   void Inform7Parser::parseDefinition(vector<string> identifiers,
   vector<string> adjectiveList) {

      // TODO
      cout << endl << "parseDefinition stub!" << endl << endl;

      cout << "Identifiers: " << endl;
      for (auto i = identifiers.begin(); i != identifiers.end(); i++) {
         cout << *i << endl;
      }

      cout << endl;
      if (adjectiveList.size()) {
         cout << "Adjectives: " << endl;
         for (auto i = adjectiveList.begin(); i != adjectiveList.end(); i++) {
            cout << *i << endl;
         }
      } else {
         cout << "(No adjectives.)" << endl;
      }

      // Skip past the rest of the definition until I actually implement this
      Token t;

      for (t = lexer.next(); PHRASE_TERMINATOR != t.type; t = lexer.next());

      t = lexer.next();

      if (QUOTED_STRING == t.type) {
         for (t = lexer.next(); PHRASE_TERMINATOR != t.type; t = lexer.next());
      } else {
         lexer.push(t);
      }
   }

   /**************************************************************************/

   void Inform7Parser::parseAdjectiveAssignment(vector<string> identifiers,
   vector<string> adjectiveList) {

      // TODO
      cout << endl << "parseAdjectiveAssignment stub!" << endl << endl;

      cout << "Identifiers: " << endl;
      for (auto i = identifiers.begin(); i != identifiers.end(); i++) {
         cout << *i << endl;
      }

      cout << endl << "Adjectives: " << endl << endl;
      for (auto i = adjectiveList.begin(); i != adjectiveList.end(); i++) {
         cout << *i << endl;
      }

      // Skip past the rest of the definition until I actually implement this
      Token t;
      for (t = lexer.next(); PHRASE_TERMINATOR != t.type; t = lexer.next());
   }

   /**************************************************************************/

   void Inform7Parser::parseEquality(vector<string> identifiers) {

      Token t;

      // Skip past any articles that might be present
      for (t = lexer.next(); ARTICLE == t.type; t = lexer.next());
      lexer.push(t);

      if (adjectives.end() != adjectives.find(strToLower(t.value))) {

         vector<string> adjectiveList;

         // If we have a list of adjectives, parse them. We're breaking away
         // from a standard recursive descent in order to avoid too much
         // lookahead.
         adjectiveList = parseAdjectivesList();

         t = lexer.next();

         if (PHRASE_TERMINATOR == t.type) {
            parseAdjectiveAssignment(identifiers, adjectiveList);
         }

         else if (WORD == t.type) {
            parseDefinition(identifiers, adjectiveList);
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
         directions.end() != directions.find(strToLower(t.value))
      ) {
         parseDefinition(identifiers);
      }

      else if (WORD == t.type || EQUALITY == t.type || AND == t.type) {
         throw ParseException(string("'") + t.value + "' is not a known adjective, kind of thing, or property of a thing (line " + to_string(t.lineno) + ')');
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

