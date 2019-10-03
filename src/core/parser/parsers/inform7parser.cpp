#include <memory>

#include "../../include/utility.h"
#include "../../include/vocabulary.h"

#include "../../include/parser/parsers/inform7parser.h"
#include "../../include/parser/parsers/inform7lexer.h"

#include "../../include/exception/parseexception.h"

namespace trogdor {


   Inform7Parser::Inform7Parser(std::unique_ptr<Instantiator> i,
   const Vocabulary &v): Parser(std::move(i), v) {

      // TODO
   }

   /**************************************************************************/

   Inform7Parser::~Inform7Parser() {

      // TODO
   }

   /**************************************************************************/

   void Inform7Parser::parse(string filename) {

      Inform7Lexer lexer(filename);

      // TODO: Just sample code right now to demonstrate that the lexer can break
      // the source code up into tokens
      cout << "Source text before tokenization: \n" << endl;

      cout << replaceAll(lexer.getSource(), "\n", "\\n\n") << endl << endl;

      cout << "Token stream: " << endl << endl;

      for (Token t = lexer.next(); t.type != SOURCE_EOF; t = lexer.next()) {

         cout << "Value: " << (0 == t.value.compare("\n") ? "\\n" : t.value) << endl;
         cout << "Value length: " << t.value.length() << endl;
         cout << "Type: ";

         switch (t.type) {

            case PUNCTUATION:
               cout << "PUNCTUATION";
               break;

            case QUOTED_STRING:
               cout << "QUOTED_STRING";
               break;

            case NEWLINE:
               cout << "NEWLINE";
               break;

            case EQUALITY:
               cout << "EQUALITY";
               break;

            case ARTICLE:
               cout << "ARTICLE";
               break;

            case WORD:
               cout << "WORD";
               break;

            default:
               cout << "UNDEFINED (this is a bug)";
               break;
         }

         cout << endl << endl;
      }

      throw ParseException("Inform 7 support not yet implemented.");
   }
}

