#ifndef TROGIN_H
#define TROGIN_H


#include <string>
#include <iostream>


using namespace std;

namespace trogdor { namespace core {


   /*
      Base input stream class.  This isn't a true input stream, but instead
      wraps around an existing one.
   */
   class Trogin {

      public:

         /*
            Returns a new instance of the input stream.  This is so that Entity
            clone constructors will work properly.

            Input:
               (none)

            Output:
               Pointer to new Trogin (actual type is of child class)
         */
         virtual Trogin *clone() = 0;

         // For now, I only need to define input for strings.  Note: contrary to
         // cin, this should always return after the user presses enter,
         // whether or not anything was typed.  If the user didn't type anything,
         // this will just be an empty string with a newline.
         virtual Trogin &operator>> (string &val) = 0;
   };
}}


#endif

