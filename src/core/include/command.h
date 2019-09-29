#ifndef COMMAND_H
#define COMMAND_H


#include <iostream>
#include <string>
#include <cstring>

#include "tokenizer.h"
#include "vocabulary.h"

#include "entities/entity.h"

using namespace std;
using namespace trogdor::entity;

namespace trogdor {


   class Game; // resolves circular dependency Game <-> Command

   class Command {

      private:

         // Reference to the game's vocabulary (used for lookups)
         const Vocabulary &vocabulary;

         // Note that a command is presumed invalid until successfully parsed
         bool invalid;

         string verb;
         string directObject;
         string indirectObject;
         string preposition;

         void parse(string commandStr);

         int parseDirectObject(Tokenizer &tokenizer);

         int parseIndirectObject(Tokenizer &tokenizer);

      public:

         /*
            Constructor for the Command class.
         */
         Command(const Vocabulary &v);

         /*
            Returns true if the command is invalid.

            Input: (none)
            Output: (none)
         */
         inline bool isInvalid() const {return invalid;}

         /*
            Returns const reference to the vocabulary.
         */
         inline const Vocabulary &getVocabulary() {return vocabulary;}

         /*
            Getters for the syntactic components of a command.
         */
         inline string getVerb() const {return verb;}
         inline string getDirectObject() const {return directObject;}
         inline string getIndirectObject() const {return indirectObject;}
         inline string getPreposition() const {return preposition;}

         /*
            Reads a command from the user.

            Input: Entity to read input from
            Output: (none)
         */
         void read(Entity *user);

         /*
            Makes Command object printable via trogout.
         */
         friend ostream &operator<<(ostream &out, const Command &c);
   };
}


#endif

