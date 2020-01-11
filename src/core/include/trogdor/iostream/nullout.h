#ifndef NULLOUT_H
#define NULLOUT_H


#include <memory>
#include <trogdor/iostream/trogout.h>


namespace trogdor {


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
         virtual std::unique_ptr<Trogout> clone();
   };
}


#endif
