#ifndef INFORM7PARSER_H
#define INFORM7PARSER_H


#include <memory>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include <trogdor/utility.h>
#include <trogdor/vocabulary.h>
#include <trogdor/languages/english.h>
#include <trogdor/exception/parseexception.h>

#include <trogdor/parser/parser.h>
#include <trogdor/parser/parsers/inform7lexer.h>


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

      <program>              ::= [<bibliographic>] <phrase> [{<phrase terminator> <phrase>}]
      <bibliographic>        ::= <quoted string> ["by" <author name>]
                                 <phrase terminator>
      <author name>          ::= /[A-Za-z ']+/ | <quoted string>
      <phrase>               ::= <equality statement> | <description stmt>
      <description stmt>     ::= {<article>} description of {<article>} <noun>
                                 <equality verb> <description>
      <equality statement>   ::= (<definition> | <property assignment> |
                                 <placement>) [<phrase terminator> <description>]
      <definition>           ::= <identifier list> <equality verb> {<article>} (
                                 [<property list>] <class> [<in clause> | 
                                 <on clause>] | <location clause>)
      <property assignment>  ::= <identifier list> <equality verb> {<article>}
                                 <property list>
      <placement>            ::= <identifier list> <equality verb> ("in" | "on")
                                 {<article>} <noun>
      <identifier list>      ::= {<article>} <noun> {("," | [","] "and")
                                 {<article>} <noun>}
      <in clause>            ::= "in" {<article>} <noun>
      <on clause>            ::= "on" {<article>} <noun>
      <location clause>      ::= <direction> ("of" | "from") {<article>} <noun>
      <description>          ::= "\"" "/^[\"]+/" ".\"" | "\"" "/^[\"]+/\""
      <equality verb>        ::= "is" | "are"
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
            std::string value;
            bool negated;
         };

         // Breaks Inform 7 source down into a token stream
         Inform7Lexer lexer;

         // All language rules are based on English
         English language;

         // Set of directions recognized by Inform 7 (list can be extended.)
         // Keys are the direction and values are the opposite. For example,
         // directions["north"] = "south".
         std::unordered_map<std::string, std::string> directions;

         // Set of classes recognized by Inform 7
         std::unordered_set<std::string> classes;

         // Plural lookup mapping plural classes to their singular equivalents
         std::unordered_map<std::string, std::string> classPlurals;

         // Set of properties (both either/or and value) recognized by Inform 7
         std::unordered_set<std::string> properties;

         // Set of non-property adjectives recognized by Inform 7
         std::unordered_set<std::string> adjectives;

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
         std::vector<std::string> parseIdentifiersList();

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
         std::vector<ParsedProperty> parsePropertyList();

         /*
            Parses the in clause of a definition. Matches the <in clause>
            production in the EBNF above.

            Input:
               One or more things that are in a container or place (std::vector<std::string>)

            Output:
               (none)
         */
         void parseInClause(std::vector<std::string> subjects);

         /*
            Parses the on clause of a definition. Matches the <on clause>
            production in the EBNF above.

            Input:
               One or more things that are on a supporter (std::vector<std::string>)

            Output:
               (none)
         */
         void parseOnClause(std::vector<std::string> subjects);

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
            Parses a thing's description.

            Input:
               All identifiers being described (if more than 1, we output an error)

            Output:
               (none)
         */
         void parseDescription(std::vector<std::string> identifiers);

         /*
            Parses the definition of one or more things. Matches the
            <definition> production in the EBNF above.

            Input:
               One or more identifiers (std::vector<std::string>)
               One or more optional properties (std::vector<ParsedProperty>)

            Output:
               (none)
         */
         void parseDefinition(std::vector<std::string> identifiers,
         std::vector<ParsedProperty> propertyList = std::vector<ParsedProperty>());

         /*
            Parses the assignment of one or more properties to one or more
            identifiers. Matches the <property assignment> production in the
            EBNF above.

            Input:
               One or more identifiers (std::vector<std::string>)
               One or more properties (std::vector<ParsedProperty>)

            Output:
               (none)
         */
         void parsePropertyAssignment(std::vector<std::string> identifiers,
         std::vector<ParsedProperty> propertyList);

         /*
            Parses the placement of one or more things into or onto a room,
            container, or supporter. Matches the <placement> production in the
            EBNF above.

            Input:
               One or more identifiers (std::vector<std::string>)

            Output:
               (none)
         */
         void parsePlacement(std::vector<std::string> subjects);

         /*
            Parses an equality phrase, or any phrase that uses the present tense
            of the verb "to be." Matches the <equality statement> production in
            the EBNF above.

            Input:
               One or more identifiers as the subject (std::vector<std::string>)

            Output:
               (none)
         */
         void parseEquality(std::vector<std::string> identifiers);

         /*
            Parses a description statement of the form "the description of
            <identifier> is "<description>". Matches the <description stmt>
            production in the EBNF above.

            Input:
               Name of the thing being described (std::string)

            Output:
               (none)
         */
         void parseDescriptionStatement(std::string identifier);

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
               New direction (std::string)
               New direction's opposite (std::string)

            Output:
               (none)
         */
         inline void insertDirection(std::string direction, std::string opposite) {

            if (directions.end() != directions.find(direction)) {
               throw ParseException(std::string("Direction '") + direction +
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
               Class name (std::string)

            Output:
               (none)
         */
         inline void insertClass(std::string className) {

            classes.insert(className);
            classPlurals[language.pluralizeNoun(className)] = className;
         }

         /*
            Inserts an Inform 7 property that can be assigned to an instance of
            a certain class.

            Input:
               Property (std::string)

            Output:
               (none)
         */
         inline void insertProperty(std::string property) {

            if (properties.end() == properties.find(property)) {
               properties.insert(property);
            }
         }

         /*
            Inserts an Inform 7 adjective.

            Input:
               Adjective (std::string)

            Output:
               (none)
         */
         inline void insertAdjective(std::string adjective) {

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
               Filename where the i7 source file is saved (std::string)

            Output:
               (none)
         */
         virtual void parse(std::string filename);
   };
}


#endif
