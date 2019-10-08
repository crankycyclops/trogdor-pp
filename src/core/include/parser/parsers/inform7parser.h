#ifndef INFORM7PARSER_H
#define INFORM7PARSER_H


#include <memory>
#include <string>
#include <vector>
#include <unordered_set>

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
                                 <sentence terminator>
      <author name>          ::= /[A-Za-z ']+/ | <quoted string>
      <phrase>               ::= <definition> | <adjective assignment>
      <definition>           ::= <identifier list> <equality> ({<article>}
                                 [<adjective>] <class> [<in clause>] |
                                 <direction> ("of" | "from") {<article>} <noun>)
                                 <sentence terminator> [<description>]
      <adjective assignment> ::= <identifier list> <equality> <adjective>
                                 <sentence terminator>
      <identifier list>      ::= {<article>} <noun> {("," | [","] "and")
                                 {<article>} <noun>}
      <in clause>            ::= "in" {<article>} <noun>
      <description>          ::= "\"" "/^[\"]+/" ".\"" [<sentence terminator>] |
                                 "\"" "/^[\"]+/\"" <sentence terminator>
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
      <adjective>            ::= "visible" | "visited" | "touchable"
      <noun>                 ::= /[A-Za-z ']+/
      <quoted string>        ::= "\" "/^[\"]+/" \""
      <sentence terminator>  ::= ("." | "\n\n") {"\n"}

      * Classes and adjectives listed in the above EBNF are those that are built
      into Inform 7. I'll eventually add support for parsing custom classes and
      adjectives, in which case the grammar should also consider those, once
      inserted, as their respective types (the same goes for custom directions.)

      Note that I had to cobble together the EBNF above myself using examples
      from the official documentation along with other supplemental sources.

      This one in particular helped a lot:
      http://www.ifwiki.org/index.php/Inform_7_for_Programmers

      And this one:
      http://mirrors.ibiblio.org/interactive-fiction/programming/inform7/manuals/Inform7_CheatSheet.pdf
   */
   class Inform7Parser: public Parser {

      private:

         // Breaks Inform 7 source down into a token stream
         Inform7Lexer lexer;

         // Set of directions recognized by Inform 7 (list can be extended)
         unordered_set<string> directions;

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

