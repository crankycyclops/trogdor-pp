#ifndef TROGERR_H
#define TROGERR_H


#include <memory>
#include <unordered_map>
#include <trogdor/iostream/trogout.h>


namespace trogdor {


   /*
      Base error output stream class. Extends Trogout to add error level
	 information.
   */
   class Trogerr: public Trogout {

      public:

         // Represents the severity of an outputed error (with ERROR being the
         // greatest and INFO being the least)
         enum ErrorLevel {
            ERROR,
            WARNING,
            INFO
         };

         /*
            Returns the EntityType corresponding to the given string.

            Input:
               Entity type name (std::string)

            Output:
               Entity type (enum EntityType)
         */
         static inline std::string errorLevelToStr(ErrorLevel l) {

            // Maps error levels to their string representations
            static std::unordered_map<ErrorLevel, std::string> levelToStrMap = {
               {ERROR,   "ERROR"},
               {WARNING, "WARNING"},
               {INFO,    "INFO"}
            };

            return levelToStrMap.find(l) != levelToStrMap.end() ?
               levelToStrMap[l] : "UNDEFINED SEVERITY";
         }

      private:

         ErrorLevel level;

      public:

         /*
            Constructor
         */
         inline Trogerr(): level(ERROR) {}

         /*
            If I don't have this, I get undefined vtable errors with code that
            links to libtrogdor.
         */
         virtual ~Trogerr();

         /*
            Returns the current error level.

            Input:
               (none)

            Output:
               Error Level (ErrorLevel)
         */
         inline ErrorLevel getErrorLevel() {return level;}

         /*
            Allows us to set the current error level.

            Input:
               Error Level (ErrorLevel)

            Output:
               (none)
         */
         inline void setErrorLevel(ErrorLevel l) {level = l;}

         /*
            This is a hack due to some perhaps bad design choices. Basically,
            when you're cloning an error stream, you should call this instead of
            clone(). Sorry for the confusion.

            A more detailed answer is that since I inherit from Trogout, and
            clone() is virtual, it must return a pointer to Trogout. However,
            classes that use this require a pointer to Trogerr. I'm thinking
            about a better way to handle this, but this is what I've come up
            with for now.

            Please, don't judge O:-)
         */
         virtual std::unique_ptr<Trogerr> copy() = 0;

         /*
            DON'T CALL THIS! See comment for copy() above.
         */
         virtual std::unique_ptr<Trogout> clone();
   };
}


#endif
