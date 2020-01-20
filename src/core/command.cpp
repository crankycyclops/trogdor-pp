#include <trogdor/command.h>
#include <trogdor/game.h>
#include <trogdor/action.h>
#include <trogdor/utility.h>

using namespace trogdor::entity;

namespace trogdor {


   Command::Command(const Vocabulary &v):
   vocabulary(v) {

      verb = "";
      directObject = "";
      indirectObject = "";
      preposition = "";

      // true if the user entered an empty command
      nullCommand = false;

      // command is invalid until we successfully parse a command
      invalid = true;
   }


   void Command::read(Entity *user) {

      std::string commandStr;

      // Prompt the user for a response
      user->out() << "\n> ";
      user->out().flush();
      user->in() >> commandStr;

      trim(commandStr);

      if (!commandStr.length()) {
         nullCommand = true;
      }

      parse(commandStr);
   }


   void Command::parse(std::string commandStr) {

      Tokenizer tokenizer(commandStr);

      // the user pressed enter without issuing a command, so ignore it
      if (tokenizer.isEnd()) {
        return;
      }

      // Start by attempting to match the entire sentence to a verb. Then,
      // continue reducing the number of words in our potential verb until
      // either we find a match or exhaust all tokens (in which case, there are
      // no verb matches and sentence validation fails.)
      std::string verbStr;
      bool verbMatched = false;

      std::vector<std::string> tokens = tokenizer.consumeAll();

      for (int numWords = tokens.size(); numWords > 0; numWords--) {

         verbStr = "";

         for (unsigned int i = 0; i < tokens.size(); i++) {
            verbStr += tokens[i] + (i < tokens.size() - 1 ? " " : "");
         }

         // Yay, we matched a verb!
         if (vocabulary.isVerb(verbStr)) {
            verb = verbStr;
            verbMatched = true;
            break;
         }

         // Haven't matched a verb yet, so backtrack by one token and try again
         else {
            tokens.pop_back();
            tokenizer.previous();
         }
      }

      // We weren't able to match a verb, so consider this command invalid
      if (!verbMatched) {
         invalid = true;
         return;
      }

      // we may have a direct and/or indirect object to look at
      if (!tokenizer.isEnd()) {

         int status;

         status = parseDirectObject(tokenizer);

         try {
            status += parseIndirectObject(tokenizer);
         }

         // check for a dangling preposition, which is a syntax error
         catch (const std::string &preposition) {

            // Hackety hack: "in" and "out" are also considered to be
            // directions, so they should alternatively be treated like direct
            // objects.
            if (0 == preposition.compare("in") || 0 == preposition.compare("out")) {
               directObject = preposition;
               status++;
            } else {
               return;
            }
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


   int Command::parseDirectObject(Tokenizer &tokenizer) {

      std::string dobj = "";
      std::string token = tokenizer.getCurToken();

      while (!token.empty() && !tokenizer.isEnd() && !vocabulary.isPreposition(token)) {

         // ignore filler words such as articles like "the" or "a"
         if (vocabulary.isFillerWord(token)) {
            tokenizer.next();
            token = tokenizer.getCurToken();
            continue;
         }

         // each token encountered is to be treated as part of the direct
         // object until or unless we encounter a preposition
         if (dobj.length() > 0) {
            dobj += " ";
         }

         dobj += token;

         tokenizer.next();
         token = tokenizer.getCurToken();
      }

      // set the command's fully assembled direct object!
      directObject = strToLower(dobj);

      // return a non-zero status if a direct object was found
      return dobj.length() > 0 ? 1 : 0;
   }


   int Command::parseIndirectObject(Tokenizer &tokenizer) {

      std::string idobj = "";
      std::string token = tokenizer.getCurToken();

      // an indirect object must be preceded by a preposition
      if (tokenizer.isEnd() || !vocabulary.isPreposition(token)) {
         return 0;
      }

      // anything after the preposition will be counted as part of the IDO
      preposition = strToLower(tokenizer.getCurToken());
      tokenizer.next();
      token = tokenizer.getCurToken();

      while (!tokenizer.isEnd()) {

         // ignore filler words
         if (vocabulary.isFillerWord(token)) {
            tokenizer.next();
            token = tokenizer.getCurToken();
            continue;
         }

         // every remaining token is to be treated as part of the IDO
         if (idobj.length() > 0) {
            idobj += " ";
         }

         idobj += token;

         tokenizer.next();
         token = tokenizer.getCurToken();
      }

      // no indirect object was parsed
      if (0 == idobj.length()) {

         // a dangling preposition is considered a syntax error
         if (preposition.length() > 0) {
            throw preposition;
         }

         return 0;
      }

      // we found a preposition + indirect object
      else {
         indirectObject = strToLower(idobj);
         return 1;
      }
   }


   // For debugging purposes; allows us to print a Command object
   std::ostream &operator<<(std::ostream &out, const Command &c) {

      std::cout << "Verb: " << c.verb << std::endl;
      std::cout << "Direct Object: " << c.directObject << std::endl;
      std::cout << "Preposition: " << c.preposition << std::endl;
      std::cout << "Indirect Object: " << c.indirectObject << std::endl;
      std::cout << "Invalid? " << c.invalid << std::endl;

      return out;
   }
}
