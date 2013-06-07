#ifndef STREAMIN_H
#define STREAMIN_H


#include "trogin.h"


using namespace std;

namespace core {


   /*
      Input "stream" that reads from the specified istream.
   */
   class StreamIn: public Trogin {

      private:

         istream *stream;

      public:

         inline StreamIn(istream *s) {stream = s;}

         /*
            See include/iostream/trogin.h for details.
         */
         virtual Trogin *clone();

         // For now, I only need to define input for strings
         virtual Trogin &operator>> (string &val);
   };
}


#endif

