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
      Parses an Inform 7 source file.
   */
   class Inform7Parser: public Parser {

      private:

         // TODO

      public:

         /*
            Constructor
         */
         inline Inform7Parser(std::unique_ptr<Instantiator> i, const Vocabulary &v):
         Parser(std::move(i), v) {

            // TODO
         }

         /*
            Destructor
         */
         ~Inform7Parser();

         /*
            Parses an Inform 7 source file and uses it to instantiate the game.
            This is an experimental work in progress, and since Trogdor-pp and
            Inform are different projects with different goals, this will likely
            never support the entire language.

            I'm writing this to learn more about grammars and parsing and
            because I think it will be a fun project :) Don't expect this to be
            particularly useful.

            Input:
               Filename where the i7 source file is saved (string)

            Output:
               (none)
         */
         virtual void parse(string filename);
   };
}


#endif

