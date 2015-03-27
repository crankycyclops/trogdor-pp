#include "include/streamout.h"


using namespace std;


void StreamOut::flush() {

   *stream << getBufferStr();
   stream->flush();
   clear();
}

core::Trogout *StreamOut::clone() {

   return new StreamOut(stream);
}

