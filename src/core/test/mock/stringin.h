#ifndef STRING_IN_H
#define STRING_IN_H


#include <trogdor/iostream/trogin.h>


// Mock class that will always return the same string (set during construction)
// as input.
class StringIn: public trogdor::Trogin {

   private:

      std::string input;

   public:

      explicit inline StringIn(std::string s) {input = s;}

   /*
      See include/iostream/trogin.h for details.
   */
   virtual std::unique_ptr<trogdor::Trogin> clone();

   // For now, I only need to define input for strings
   virtual trogdor::Trogin &operator>> (std::string &val);
};


#endif

