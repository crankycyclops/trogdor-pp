#ifndef NULLOUT_H
#define NULLOUT_H


#include "trogout.h"


using namespace std;

namespace core {


   /*
      Stub output "stream" that writes nothing.
   */
   class NullOut: public Trogout {

      protected:

         /*
            See include/iostream/trogout.h for details.
         */
         virtual void flush();

      public:

         /*
            See include/iostream/trogout.h for details.
         */
         virtual Trogout *clone();
   };
}


#endif

