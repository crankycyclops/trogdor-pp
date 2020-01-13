#ifndef NULLERR_H
#define NULLERR_H


#include <memory>
#include <trogdor/iostream/trogerr.h>


namespace trogdor {


   /*
      Stub error output "stream" that writes nothing.
   */
   class NullErr: public Trogerr {

      public:

         /*
            See include/iostream/trogout.h for details.
         */
         virtual void flush();

         /*
            See include/iostream/trogerr.h for details.
         */
         virtual std::unique_ptr<Trogerr> copy();
   };
}


#endif
