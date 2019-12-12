#ifndef STREAMIN_H
#define STREAMIN_H


#include <memory>
#include "../../core/include/iostream/trogin.h"



/*
   Input "stream" that reads from the specified istream.
*/
class StreamIn: public trogdor::Trogin {

   private:

      std::istream *stream;

   public:

      inline StreamIn(std::istream *s) {stream = s;}

   /*
      See include/iostream/trogin.h for details.
   */
   virtual std::unique_ptr<trogdor::Trogin> clone();

   // For now, I only need to define input for strings
   virtual trogdor::Trogin &operator>> (std::string &val);
};


#endif

