#include <ctime>
#include <iomanip>
#include <memory>

#include "../../include/io/iostream/streamerr.h"


StreamErr::~StreamErr() {}

/*****************************************************************************/

void StreamErr::flush() {

   auto t = std::time(nullptr);
   auto tm = *std::localtime(&t);

   *stream << '[' << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "] " <<
      Trogerr::errorLevelToStr(getErrorLevel()) << ": " << getBufferStr();
   stream->flush();
}

/*****************************************************************************/

std::unique_ptr<trogdor::Trogerr> StreamErr::copy() {

   return std::make_unique<StreamErr>(stream);
}
