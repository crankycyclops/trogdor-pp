#ifndef COMMAND_H
#define COMMAND_H


#include <iostream>
#include <string>
#include <cstring>

#include <trogdor/tokenizer.h>
#include <trogdor/vocabulary.h>

#include <trogdor/entities/entity.h>

using namespace trogdor::entity;

namespace trogdor {


   class Game; // resolves circular dependency Game <-> Command

   class Command {

      private:

         // Reference to the game's vocabulary (used for lookups)
         const Vocabulary &vocabulary;

         // True if the user entered an empty command
         bool nullCommand;

         // Note that a command is presumed invalid until successfully parsed
         bool invalid;

         std::string verb;
         std::string directObject;
         std::string indirectObject;
         std::string preposition;

         void parse(std::string commandStr);

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
            Returns true if the user entered an empty command.

            Input: (none)
            Output: (none)
         */
         inline bool isNull() const {return nullCommand;}

         /*
            Returns const reference to the vocabulary.
         */
         inline const Vocabulary &getVocabulary() {return vocabulary;}

         /*
            Getters for the syntactic components of a command.
         */
         inline std::string getVerb() const {return verb;}
         inline std::string getDirectObject() const {return directObject;}
         inline std::string getIndirectObject() const {return indirectObject;}
         inline std::string getPreposition() const {return preposition;}

         /*
            Reads a command from the user.

            Input: Entity to read input from
            Output: (none)
         */
         void read(Entity *user);

         /*
            Makes Command object printable via trogout.
         */
         friend std::ostream &operator<<(std::ostream &out, const Command &c);
   };
}


#endif
