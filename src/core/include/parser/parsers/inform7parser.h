#ifndef INFORM7PARSER_H
#define INFORM7PARSER_H


#include <memory>
#include <string>

#include "../../utility.h"
#include "../../vocabulary.h"

#include "../parser.h"


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

      <sentence>             ::= <rule>
      <rule>                 ::= <singular definition> | <plural definition> |
                                 <adjective assignment>
      <singular definition>  ::= [<article>] <noun> <equality> ([<article>]
                                 [<adjective>] <class> [<in clause>] |
                                 <direction> ("of" | "from") [<article>] <noun>)
                                 "." [<description>
      <plural definition>    ::= [<article>] <noun>{"," [<article>] <noun>}[","]
                                 "and" [<article>] <noun> <equality> [<adjective>]
                                 <plural class> [<in clause>] "."
      <adjective assignment> ::= [<article>] <noun> <equality> <adjective> | [<article>]
                                 <noun>{"," [<article>] <noun>}[","] "and"
                                 [<article>] <noun> <equality> <adjective>
      <in clause>            ::= "in" [<article>] <noun>
      <description>          ::= "\""/^[\"]+/".\""["."] | "\""/^[\"]+/["."]"\"."
      <noun>                 ::= /[A-Za-z ']+/
      <adjective>            ::= /[A-Za-z ]+/
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

      * Classes included above are those that are built into Inform 7. Once I
      add support for custom classes, I'll also have to be able to parse those.
      The same goes for custom directions, synonyms, etc.

      Note that I had to cobble together the EBNF above myself using examples
      from the official documentation along with other supplemental sources.

      This one in particular helped a lot:
      http://www.ifwiki.org/index.php/Inform_7_for_Programmers
   */
   class Inform7Parser: public Parser {

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

