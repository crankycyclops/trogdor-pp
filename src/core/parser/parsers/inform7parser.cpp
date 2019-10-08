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
      // extended later.
      directions.insert("north");
      directions.insert("south");
      directions.insert("east");
      directions.insert("west");
      directions.insert("northwest");
      directions.insert("northeast");
      directions.insert("southwest");
      directions.insert("southeast");
      directions.insert("up");
      directions.insert("down");
      directions.insert("inside");
      directions.insert("outside");

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
            t.type != SENTENCE_TERMINATOR &&
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

      else if (SENTENCE_TERMINATOR != t.type) {
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
         // the and.
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

   void Inform7Parser::parsePhrase() {

      Token t;

      // We're going to break away from strict LL parsing for a bit, because
      // otherwise there would be too much lookahead
      vector<string> identifiers = parseIdentifiersList();

      t = lexer.next();

/* This breaks because I don't handle descriptions right after definitions yet.
      if (EQUALITY != t.type) {
         throw ParseException(string("I can't find a verb that I know how to deal with. (line ") + to_string(t.lineno) + ')');
      }
*/
      // TODO: actually do something
      for (int i = 0; i < identifiers.size(); i++) {
         cout << identifiers[i] << endl;
      }

      // TODO: this is just skipping past the rest of the phrase until I implement
      // the rest
      for (Token t = lexer.next(); SOURCE_EOF != t.type && SENTENCE_TERMINATOR != t.type;
      t = lexer.next());
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

