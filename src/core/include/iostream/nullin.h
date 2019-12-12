#ifndef NULLIN_H
#define NULLIN_H


#include <memory>
#include "trogin.h"


namespace trogdor {


   /*
      Input "stream" that doesn't actually read anything.
   */
   class NullIn: public Trogin {

      public:

         /*
            See include/iostream/trogin.h for details.
         */
         virtual std::unique_ptr<Trogin> clone();

         // For now, I only need to define input for strings
         virtual Trogin &operator>> (std::string &val);
   };
}


#endif
