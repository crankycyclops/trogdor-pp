#include <memory>
#include "include/streamout.h"


void StreamOut::flush() {

   *stream << getBufferStr();
   stream->flush();
   clear();
}

std::unique_ptr<trogdor::Trogout> StreamOut::clone() {

   return std::make_unique<StreamOut>(stream);
}
