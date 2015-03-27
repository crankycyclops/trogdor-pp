#ifndef STREAMIN_H
#define STREAMIN_H


#include "../../core/include/iostream/trogin.h"


using namespace std;


/*
 Input "stream" that reads from the specified istream.
*/
class StreamIn: public core::Trogin {

   private:

      istream *stream;

   public:

      inline StreamIn(istream *s) {stream = s;}

      /*
       See include/iostream/trogin.h for details.
      */
      virtual core::Trogin *clone();

      // For now, I only need to define input for strings
      virtual core::Trogin &operator>> (string &val);
};


#endif

