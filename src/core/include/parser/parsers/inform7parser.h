#ifndef INFORM7PARSER_H
#define INFORM7PARSER_H


#include <memory>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include "../../utility.h"
#include "../../vocabulary.h"

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
      <phrase>               ::= <definition> | <adjective assignment>
      <definition>           ::= <identifier list> <equality> {<article>} (
                                 [<adjective list>] <class> [<in clause>] |
                                 <direction> ("of" | "from") {<article>} <noun>)
                                 <phrase terminator> [<description>]
      <adjective assignment> ::= <identifier list> <equality> {<article>}
                                 <adjective list>
                                 <phrase terminator>
      <identifier list>      ::= {<article>} <noun> {("," | [","] "and")
                                 {<article>} <noun>}
      <in clause>            ::= "in" {<article>} <noun>
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
      <adjective>            ::= ["not"] ("adjacent" | "visible" | "invisible" |
                                 "visited" | "touchable" | "untouchable" |
                                 "lighted" | "dark" | "open" | "closed" |
                                 "empty" | "non-empty" | "lit" | "unlit" |
                                 "transparent" | "opaque" | "fixed in place" |
                                 "portable" | "openable" | "unopenable" |
                                 "enterable" | "pushable between rooms" |
                                 "wearable" | "edible" | "inedible" |
                                 "described" | "undescribed" | "male" |
                                 "female" | "neuter" | "locked" | "unlocked" |
                                 "lockable" | "unlockable" | "switched on" |
                                 "switched off" | "positive" | "negative" |
                                 "even" | "odd")
      <noun>                 ::= /[A-Za-z ']+/
      <quoted string>        ::= "\" "/^[\"]+/" \""
      <phrase terminator>    ::= ("." | "\n\n") {"\n"}

      * Directions, classes, and adjectives listed in the above EBNF are those
      that are built into Inform 7. I'll eventually add support for parsing
      custom classes and adjectives, in which case the grammar should also
      consider those, once inserted, as their respective types.

      Note that I had to cobble together the EBNF above myself using examples
      from the official documentation along with other supplemental sources.

      This one in particular helped a lot:
      http://www.ifwiki.org/index.php/Inform_7_for_Programmers

      And this one:
      http://mirrors.ibiblio.org/interactive-fiction/programming/inform7/manuals/Inform7_CheatSheet.pdf

      Where possible, I try to throw the same (or similar) error messages as
      those returned by the original Inform 7 compiler.
   */
   class Inform7Parser: public Parser {

      private:

         // Breaks Inform 7 source down into a token stream
         Inform7Lexer lexer;

         // Set of directions recognized by Inform 7 (list can be extended.)
         // Keys are the direction and values are the opposite. For example,
         // directions["north"] = "south".
         unordered_map<string, string> directions;

         // Set of classes recognized by Inform 7 (list can be extended)
         unordered_set<string> classes;

        // Set of adjectives recognized by Inform 7 (list can be extended)
         unordered_set<string> adjectives;

         /*
            Parses one or more identifiers on the left hand side of an equality.
            Matches the <identifiers list> production in the EBNF above. This
            method is kind of a cheat and deviates from the LL parsing pattern.
            If I don't do this, I'll have far too much lookahead. The result of
            this method will either be passed to parseDefinition() or
            parseAdjectiveAssignment(), depending on which phrase ends up being
            matched.

            Input:
               (none)

            Output:
               Vector containing one or more identifiers.
         */
         vector<string> parseIdentifiersList();

         /*
            Parses one or more adjectives on the left hand side of an equality.
            Matches the <adjective list> production in the EBNF above. Like
            parseIdentifiersList(), this method is kind of a cheat. It deviates
            from the LL parsing pattern so that I won't have too much
            lookahead.

            Input:
               (none)

            Output:
               Vector containing one or more adjectives.
         */
         vector<string> parseAdjectivesList();

         /*
            Parses the definition of one or more things. Matches the
            <definition> production in the EBNF above.

            Input:
               One or more identifiers (vector<string>)
               One or more optional adjectives (vector<string>)

            Output:
               (none)
         */
         void parseDefinition(vector<string> identifiers,
         vector<string> adjectiveList = vector<string>());

         /*
            Parses the assignment of one or more adjectives to one or more
            identifiers. Matches the <adjective assignment> production in the
            EBNF above.

            Input:
               One or more identifiers (vector<string>)
               One or more adjectives (vector<string>)

            Output:
               (none)
         */
         void parseAdjectiveAssignment(vector<string> identifiers,
         vector<string> adjectiveList);

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

