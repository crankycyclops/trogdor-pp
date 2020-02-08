#include <memory>
#include "../include/iostream/streamerr.h"


StreamErr::~StreamErr() {}

void StreamErr::flush() {

   *stream << Trogerr::errorLevelToStr(getErrorLevel()) << ": " << getBufferStr();
   stream->flush();
}

std::unique_ptr<trogdor::Trogerr> StreamErr::copy() {

   return std::make_unique<StreamErr>(stream);
}
