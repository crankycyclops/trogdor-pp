#ifndef CONSOLEIN_H
#define CONSOLEIN_H


#include "trogin.h"


using namespace std;

namespace core {


   /*
      Input "stream" that reads from stdin.
   */
   class ConsoleIn: public Trogin {

      public:

         /*
            See include/iostream/trogin.h for details.
         */
         virtual Trogin *clone();

         // For now, I only need to define input for strings
         virtual Trogin &operator>> (string &val);
   };
}


#endif

