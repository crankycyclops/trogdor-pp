#ifndef NULLOUT_H
#define NULLOUT_H


#include "trogout.h"


using namespace std;

namespace trogdor { namespace core {


   /*
      Stub output "stream" that writes nothing.
   */
   class NullOut: public Trogout {

      public:

         /*
            See include/iostream/trogout.h for details.
         */
         virtual void flush();

         /*
            See include/iostream/trogout.h for details.
         */
         virtual Trogout *clone();
   };
}}


#endif

