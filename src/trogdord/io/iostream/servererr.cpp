#include <ctime>
#include <iomanip>
#include <memory>

#include <trogdord/io/iostream/servererr.h>


ServerErr::~ServerErr() {}

/*****************************************************************************/

void ServerErr::flush() {

   auto t = std::time(nullptr);
   auto tm = *std::localtime(&t);

   *stream << '[' << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "] " <<
      Trogerr::errorLevelToStr(getErrorLevel()) << ": " << getBufferStr();
   stream->flush();
}

/*****************************************************************************/

std::unique_ptr<trogdor::Trogerr> ServerErr::copy() {

   return std::make_unique<ServerErr>(stream);
}
