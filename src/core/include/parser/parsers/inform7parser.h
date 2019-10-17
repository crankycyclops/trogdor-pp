#ifndef INFORM7PARSER_H
#define INFORM7PARSER_H


#include <memory>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include "../../utility.h"
#include "../../vocabulary.h"
#include "../../languages/english.h"
#include "../../exception/parseexception.h"

#include "../parser.h"
#include "inform7lexer.h"


using namespace std;

namespace trogdor {

   /*
      Parses an Inform 7 source file and uses it to instantiate the game. This
      is an experimental work in progress, and since Trogdor-pp and Inform are
      different projects with different goals, this will likely never support
      the entire language.

      I'm writing this to learn more about grammars and parsing and because I
      think it will be a fun project :) Don't expect this to be particularly
      useful.

      Here's the current subset of the language I'm parsing in EBNF format (I'll
      keep this updated as support for additional language features grows):

      <program>              ::= [<bibliographic>] {<rule>}
      <bibliographic>        ::= <quoted string> ["by" <author name>]
                                 <phrase terminator>
      <author name>          ::= /[A-Za-z ']+/ | <quoted string>
      <phrase>               ::= <equality>
      <equality>             ::= <definition> | <property assignment>
      <definition>           ::= <identifier list> <equality> {<article>} (
                                 [<property list>] <class> [<in clause> | 
                                 <on clause>] | <location clause>)
                                 <phrase terminator> [<description>]
      <property assignment>  ::= <identifier list> <equality> {<article>}
                                 <property list>
                                 <phrase terminator>
      <identifier list>      ::= {<article>} <noun> {("," | [","] "and")
                                 {<article>} <noun>}
      <in clause>            ::= "in" {<article>} <noun>
      <on clause>            ::= "on" {<article>} <noun>
      <location clause>      ::= <direction> ("of" | "from") {<article>} <noun>
      <description>          ::= "\"" "/^[\"]+/" ".\"" [<phrase terminator>] |
                                 "\"" "/^[\"]+/\"" <phrase terminator>
      <equality>             ::= "is" | "are"
      <assertion verb>       ::= "has" | "carries" | "is carrying" | "wears" |
                                 "is wearing" | "contains" | "supports" |
                                 "is supporting"
      <article>              ::= "a" | "an" | "the"
      <direction>            ::= "north" | "northeast" | "east" | "southeast" |
                                 "south" | "southwest" | "west" | "northwest" |
                                 "up" | "down" | "inside" | "outside"
      <class>                ::= "object" | "direction" | "room" | "region" |
                                 "thing" | "door" | "container" | "vehicle" |
                                 "player's holdall" | "supporter" | "backdrop" |
                                 "device" | "person" | "man" | "woman" | "animal"
      <plural class>         ::= "objects" | "directions" | "rooms" |
                                 "regions" | "things" | "doors" | "containers" |
                                 "vehicles" | "player's holdalls" |
                                 "supporters" | "backdrops" | "devices" |
                                 "people" | "men" | "women" | "animals"
      <adjective list>       ::= <adjective> {[","] ["and"] <adjective>}
      <adjective>            ::= ["not"] (<property> | ("adjacent" | "visible" |
                                 "invisible" | "visited" | "empty" |
                                 "non-empty" | "carried" | "positive" |
                                 "negative" | "even" | "odd"))
      <property list>        ::= <property> {[","] ["and"] <property>}
      <property>             ::= ["not"] ("touchable" | "untouchable" |
                                 "lighted" | "dark" | "open" | "closed" |
                                 "lit" | "unlit" | "transparent" | "opaque" |
                                 "fixed in place" | "portable" | "openable" |
                                 "unopenable" | "enterable" | "wearable" |
                                 "edible" | "inedible" | "described" |
                                 "undescribed" | "male" | "female" | "neuter" |
                                 "pushable between rooms" | "locked" |
                                 "unlocked" | "lockable" | "unlockable" |
                                 "switched on" | "switched off")
      <noun>                 ::= /[A-Za-z ']+/
      <quoted string>        ::= "\" "/^[\"]+/" \""
      <phrase terminator>    ::= ("." | "\n\n") {"\n"}

      * Directions, classes, properties, and adjectives listed in the above EBNF
      are those that are built into Inform 7. I'll eventually add support for
      parsing custom classes and adjectives, in which case the grammar should
      also consider those, once inserted, as their respective types.

      Note that I had to cobble together the EBNF above myself using examples
      from the official documentation (http://inform7.com/book/WI_1_1.html)
      along with other supplemental sources.

      This one in particular helped a lot:
      http://www.ifwiki.org/index.php/Inform_7_for_Programmers

      And this one:
      http://mirrors.ibiblio.org/interactive-fiction/programming/inform7/manuals/Inform7_CheatSheet.pdf

      Where possible, I try to throw the same (or similar) error messages as
      those returned by the original Inform 7 compiler.
   */
   class Inform7Parser: public Parser {

      private:

         // Data type that stores a parsed property and whether or not it's
         // negated (prefixed by "not")
         struct ParsedProperty {
            string value;
            bool negated;
         };

         // Breaks Inform 7 source down into a token stream
         Inform7Lexer lexer;

         // All language rules are based on English
         English language;

         // Set of directions recognized by Inform 7 (list can be extended.)
         // Keys are the direction and values are the opposite. For example,
         // directions["north"] = "south".
         unordered_map<string, string> directions;

         // Set of classes recognized by Inform 7
         unordered_set<string> classes;

         // Plural lookup mapping plural classes to their singular equivalents
         unordered_map<string, string> classPlurals;

         // Set of properties (both either/or and value) recognized by Inform 7
         unordered_set<string> properties;

         // Set of non-property adjectives recognized by Inform 7
         unordered_set<string> adjectives;

         /*
            Parses one or more identifiers on the left hand side of an equality.
            Matches the <identifiers list> production in the EBNF above. This
            method is kind of a cheat and deviates from the LL parsing pattern.
            If I don't do this, I'll have far too much lookahead. The result of
            this method will either be passed to parseDefinition() or
            parsePropertyAssignment(), depending on which phrase ends up being
            matched.

            Input:
               (none)

            Output:
               Vector containing one or more identifiers.
         */
         vector<string> parseIdentifiersList();

         /*
            Parses one or more properties on the left hand side of an equality.
            Matches the <property list> production in the EBNF above. Like
            parseIdentifiersList(), this method is kind of a cheat. It deviates
            from the LL parsing pattern so that I won't have too much
            lookahead.

            Input:
               (none)

            Output:
               Vector containing one or more properties
         */
         vector<ParsedProperty> parsePropertyList();

         /*
            Parses the in clause of a definition. Matches the <in clause>
            production in the EBNF above.

            Input:
               One or more things that are in a container or place (vector<string>)

            Output:
               (none)
         */
         void parseInClause(vector<string> subjects);

         /*
            Parses the location clause of a definition. Matches the <location
            clause> production in the EBNF above.

            Input:
               (none)

            Output:
               (none)
         */
         void parseLocationClause();

         /*
            Parses the definition of one or more things. Matches the
            <definition> production in the EBNF above.

            Input:
               One or more identifiers (vector<string>)
               One or more optional properties (vector<ParsedProperty>)

            Output:
               (none)
         */
         void parseDefinition(vector<string> identifiers,
         vector<ParsedProperty> propertyList = vector<ParsedProperty>());

         /*
            Parses the assignment of one or more properties to one or more
            identifiers. Matches the <property assignment> production in the
            EBNF above.

            Input:
               One or more identifiers (vector<string>)
               One or more properties (vector<ParsedProperty>)

            Output:
               (none)
         */
         void parsePropertyAssignment(vector<string> identifiers,
         vector<ParsedProperty> propertyList);

         /*
            Parses an equality phrase, or any phrase that uses the present tense
            of the verb "to be." Matches the <equality> production in the EBNF
            above.

            Input:
               One or more identifiers as the subject (vector<string>)

            Output:
               (none)
         */
         void parseEquality(vector<string> identifiers);

         /*
            Parses the optional bibliographic sentence at the beginning of the
            source code. Corresponds to the <bibliographic> production in the
            EBNF above.

            Input:
               (none)

            Output:
               (none)
         */
         void parseBibliographic();

         /*
            Parses a phrase. Corresponds to the <phrase> production in the EBNF
            above.

            Input:
               (none)

            Output:
               (none)
         */
         void parsePhrase();

         /*
            Parses an Inform 7 program. Corresponds to the <program> production
            in the EBNF above.

            Input:
               (none)

            Output:
               (none)
         */
         void parseProgram();

      protected:

         /*
            Inserts a direction recognized by Inform 7. Takes as input both the
            new direction and its opposite. If the opposite direction doesn't
            exist, it will be inserted, too, and the new direction will be set
            as its opposite.

            Example call: insertDirection("north", "south");

            Input:
               New direction (string)
               New direction's opposite (string)

            Output:
               (none)
         */
         inline void insertDirection(string direction, string opposite) {

            if (directions.end() != directions.find(direction)) {
               throw ParseException(string("Direction '") + direction +
                  "' has already been defined.");
            }

            directions[direction] = opposite;

            if (directions.end() == directions.find(opposite)) {
               directions[opposite] = direction;
            }
         }

         /*
            Inserts an Inform 7 class.

            Input:
               Class name (string)

            Output:
               (none)
         */
         inline void insertClass(string className) {

            classes.insert(className);
            classPlurals[language.pluralizeNoun(className)] = className;
         }

         /*
            Inserts an Inform 7 property that can be assigned to an instance of
            a certain class.

            Input:
               Property (string)

            Output:
               (none)
         */
         inline void insertProperty(string property) {

            if (properties.end() == properties.find(property)) {
               properties.insert(property);
            }
         }

         /*
            Inserts an Inform 7 adjective.

            Input:
               Adjective (string)

            Output:
               (none)
         */
         inline void insertAdjective(string adjective) {

            if (adjectives.end() == adjectives.find(adjective)) {
               adjectives.insert(adjective);
            }
         }

      public:

         /*
            Constructor
         */
         Inform7Parser(std::unique_ptr<Instantiator> i, const Vocabulary &v);

         /*
            Destructor
         */
         ~Inform7Parser();

         /*
            Parses an Inform 7 source file.

            Input:
               Filename where the i7 source file is saved (string)

            Output:
               (none)
         */
         virtual void parse(string filename);
   };
}


#endif

