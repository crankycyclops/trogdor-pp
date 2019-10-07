#ifndef INFORM7PARSER_H
#define INFORM7PARSER_H


#include <memory>
#include <string>

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
      <rule>                 ::= <singular definition> | <plural definition> |
                                 <adjective assignment>
      <singular definition>  ::= {<article>} <noun> <equality> ({<article>}
                                 [<adjective>] <class> [<in clause>] |
                                 <direction> ("of" | "from") {<article>} <noun>)
                                 <sentence terminator> [<description>]
      <plural definition>    ::= {<article>} <noun>{"," {<article>} <noun>}[","]
                                 "and" {<article>} <noun> <equality> [<adjective>]
                                 <plural class> [<in clause>] <sentence terminator>
      <adjective assignment> ::= {<article>} <noun> <equality> <adjective> | {<article>}
                                 <noun>{"," {<article>} <noun>}[","] "and"
                                 {<article>} <noun> <equality> <adjective>
                                 <sentence terminator>
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
      <noun>                 ::= /[A-Za-z ']+/
      <adjective>            ::= /[A-Za-z ]+/
      <quoted string>        ::= "\" "/^[\"]+/" \""
      <sentence terminator>  ::= ("." | "\n\n") {"\n"}

      * Classes included above are those that are built into Inform 7. Once I
      add support for custom classes, I'll also have to be able to parse those.
      The same goes for custom directions, synonyms, etc.

      Note that I had to cobble together the EBNF above myself using examples
      from the official documentation along with other supplemental sources.

      This one in particular helped a lot:
      http://www.ifwiki.org/index.php/Inform_7_for_Programmers
   */
   class Inform7Parser: public Parser {

      private:

         // Breaks Inform 7 source down into a token stream
         Inform7Lexer lexer;

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
            Parses a rule. Corresponds to the <rule> production in the EBNF
            above.

            Input:
               (none)

            Output:
               (none)
         */
         void parseRule();

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

