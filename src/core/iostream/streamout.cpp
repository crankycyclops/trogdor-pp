#include "../include/iostream/streamout.h"


using namespace std;

namespace core {


   void StreamOut::flush() {

      *stream << getBufferStr();
      stream->flush();
      clear();
   }

   Trogout *StreamOut::clone() {

      return new StreamOut(stream);
   }
}

