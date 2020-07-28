#ifndef COMMAND_H
#define COMMAND_H


#include <optional>
#include <string>
#include <cstring>

#include <trogdor/tokenizer.h>
#include <trogdor/vocabulary.h>

namespace trogdor {


   class Game; // resolves circular dependency Game <-> Command

   namespace entity {

      class Entity; // resolves circular dependency Game <-> Entity
   }

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

         // Optional quantities that were specified along with the direct or
         // indirect objects
         std::optional<double> directObjectQty = std::nullopt;
         std::optional<double> indirectObjectQty = std::nullopt;

         void parse(std::string commandStr);

         int parseDirectObject(Tokenizer &tokenizer);

         int parseIndirectObject(Tokenizer &tokenizer);

      public:

         /*
            Constructor and assignment operator for the Command class.
         */
         Command() = delete;
         Command(const Vocabulary &v, std::string commandStr);

         // Allows us to construct arbitrary commands
         inline Command(
            const Vocabulary &vocabulary,
            std::string verb,
            std::string directObject,
            std::string indirectObject = "",
            std::string preposition = "",
            std::optional<double> directObjectQty = std::nullopt,
            std::optional<double> indirectObjectQty = std::nullopt
         ): vocabulary(vocabulary), verb(verb), directObject(directObject),
         indirectObject(indirectObject), preposition(preposition),
         directObjectQty(directObjectQty), indirectObjectQty(indirectObjectQty) {

            nullCommand = verb.length() ? true : false;

            // Note that it's possible to construct invalid commands and we
            // won't be able to check easily for validity. I'm assuming that
            // if you have a reason to build a Command manually, you also know
            // how to do so correctly. Not passing in valid parameters will
            // result in undefined behavior.
            invalid = false;
         }

         inline Command(const Command &c):
            vocabulary(c.vocabulary),
            nullCommand(c.nullCommand),
            invalid(c.invalid),
            verb(c.verb),
            directObject(c.directObject),
            indirectObject(c.indirectObject),
            preposition(c.preposition),
            directObjectQty(c.directObjectQty),
            indirectObjectQty(c.indirectObjectQty) {}

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
         inline const Vocabulary &getVocabulary() const {return vocabulary;}

         /*
            Getters for the syntactic components of a command.
         */
         inline std::string getVerb() const {return verb;}
         inline std::string getDirectObject() const {return directObject;}
         inline std::string getIndirectObject() const {return indirectObject;}
         inline std::string getPreposition() const {return preposition;}
         inline std::optional<double> getDirectObjectQty() const {return directObjectQty;}
         inline std::optional<double> getIndirectObjectQty() const {return indirectObjectQty;}

         /*
            Makes Command object printable via trogout.
         */
         friend std::ostream &operator<<(std::ostream &out, const Command &c);
   };
}


#endif
