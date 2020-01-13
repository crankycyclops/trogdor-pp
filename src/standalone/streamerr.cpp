#include <memory>
#include <trogdor/iostream/trogout.h>
#include "include/streamerr.h"


StreamErr::~StreamErr() {}

void StreamErr::flush() {

   *stream << Trogerr::errorLevelToStr(getErrorLevel()) << ": " << getBufferStr();
   stream->flush();
   clear();
}

std::unique_ptr<trogdor::Trogerr> StreamErr::copy() {

   return std::make_unique<StreamErr>(stream);
}
