#ifndef CONSOLEOUT_H
#define CONSOLEOUT_H


#include "trogout.h"


using namespace std;

namespace core {


   /*
      Output "stream" that writes to stdout via cout.
   */
   class ConsoleOut: public Trogout {

      protected:

         /*
            See include/iostream/trogout.h for details.
         */
         virtual void flush();
   };
}


#endif

