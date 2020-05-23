#ifndef STREAMOUT_H
#define STREAMOUT_H


#include <memory>
#include <unordered_set>
#include <trogdor/iostream/trogout.h>



/*
   Output "stream" that wraps around the specified ostream object. This
   particular object happens to ignore channels.
*/
class StreamOut: public trogdor::Trogout {

   private:

      // These are channels we should ignore when sending output
      std::unordered_set<std::string> ignoredChannels;

      // output stream where flush() sends data
      std::ostream *stream;

   public:

      StreamOut(std::ostream *s);

      /*
         See include/iostream/trogout.h for details.
      */
      virtual void flush();

      /*
         See include/iostream/trogout.h for details.
      */
      virtual std::unique_ptr<trogdor::Trogout> clone();
};


#endif
