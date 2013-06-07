#include "../include/iostream/streamout.h"


using namespace std;

namespace core {


   void StreamOut::flush() {

      *stream << getBufferStr();
      stream->flush();
   }

   Trogout *StreamOut::clone() {

      return new StreamOut(stream);
   }
}

