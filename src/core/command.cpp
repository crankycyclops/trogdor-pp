#include "include/command.h"
#include "include/game.h"
#include "include/actionmap.h"
#include "include/action.h"
#include "include/utility.h"

using namespace std;

namespace core {


   Command::Command() {

      verb = "";
      directObject = "";
      indirectObject = "";
      preposition = "";

      // command is invalid until we successfully parse a command
      invalid = true;
   }


   void Command::read(istream &in, ostream &out) {

      string commandStr;

      // prompt the user until we get a response
      do {
         out << "> ";
         getline(in, commandStr);
      } while (0 == commandStr.length());

      parse(commandStr);
   }


   void Command::parse(string commandStr) {

      Tokenizer *tokenizer = new Tokenizer(commandStr);

      // the user pressed enter without issuing a command, so ignore it
      if (tokenizer->isEnd()) {
        return;
      }

      // the first token will always be considered the "verb"
      verb = strToLower(tokenizer->getCurToken());

      // we may have a direct and/or indirect object to look at
      tokenizer->next();

      if (!tokenizer->isEnd()) {

         int status;

         status  = parseDirectObject(tokenizer);

         // check for a dangling preposition, which is a syntax error
         try {
            status += parseIndirectObject(tokenizer);
         } catch (bool i) {
            return;
         }

         // if we detected additional characters, but don't have a direct object
         // or indirect object, then we have a syntax error
         if (!status) {
            return;
         }
      }

      // we're done parsing!
      invalid = false;
      return;
   }


   int Command::parseDirectObject(Tokenizer *tokenizer) {

      string dobj = "";
      string token = tokenizer->getCurToken();

      while (!tokenizer->isEnd() && !isPreposition(token)) {

         // ignore filler words such as articles like "the" or "a"
         if (isFillerWord(token)) {
            tokenizer->next();
            token = tokenizer->getCurToken();
            continue;
         }

         // each token encountered is to be treated as part of the direct
         // object until or unless we encounter a preposition
         if (dobj.length() > 0) {
            dobj += " ";
         }

         dobj += token;

         tokenizer->next();
         token = tokenizer->getCurToken();
      }

      // set the command's fully assembled direct object!
      directObject = strToLower(dobj);

      // return a non-zero status if a direct object was found
      return dobj.length() > 0 ? 1 : 0;
   }


   int Command::parseIndirectObject(Tokenizer *tokenizer) {

      string idobj = "";
      string token = tokenizer->getCurToken();

      // an indirect object must be preceded by a preposition
      if (tokenizer->isEnd() || !isPreposition(token)) {
         return 0;
      }

      // anything after the preposition will be counted as part of the IDO
      preposition = strToLower(tokenizer->getCurToken());
      tokenizer->next();
      token = tokenizer->getCurToken();

      while (!tokenizer->isEnd()) {

         // ignore filler words
         if (isFillerWord(token)) {
            tokenizer->next();
            token = tokenizer->getCurToken();
            continue;
         }

         // every remaining token is to be treated as part of the IDO
         if (idobj.length() > 0) {
            idobj += " ";
         }

         idobj += token;

         tokenizer->next();
         token = tokenizer->getCurToken();
      }

      // no indirect object was parsed
      if (0 == idobj.length()) {

         // a dangling preposition is considered a syntax error
         if (preposition.length() > 0) {
            throw false;
         }

         return 0;
      }

      // we found a preposition + indirect object
      else {
         indirectObject = strToLower(idobj);
         return 1;
      }
   }


   bool Command::isPreposition(const string word) {

      // g_prepositions defined in vocabulary.cpp
      for (int i = 0; g_prepositions[i] != 0; i++) {
         if (0 == strcmp(word.c_str(), g_prepositions[i])) {
            return 1;
         }
      }

      return 0;
   }


   bool Command::isFillerWord(const string word) {

      // g_fillerWords defined in vocabulary.cpp
      for (int i = 0; g_fillerWords[i] != 0; i++) {
         if (0 == strcmp(word.c_str(), g_fillerWords[i])) {
            return 1;
         }
      }

      return 0;
   }


   // For debugging purposes; allows us to print a Command object
   ostream &operator<<(ostream &out, const Command &c) {

      cout << "Verb: " << c.verb << endl;
      cout << "Direct Object: " << c.directObject << endl;
      cout << "Preposition: " << c.preposition << endl;
      cout << "Indirect Object: " << c.indirectObject << endl;
      cout << "Invalid? " << c.invalid << endl;

      return out;
   }
}

