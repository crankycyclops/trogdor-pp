#include <memory>
#include "include/streamout.h"


StreamOut::StreamOut(std::ostream *s): trogdor::Trogout(), stream(s) {

   ignoredChannels.insert("location");
   ignoredChannels.insert("health");
}

void StreamOut::flush() {

   if (ignoredChannels.end() == ignoredChannels.find(getChannel())) {
      *stream << getBufferStr();
      stream->flush();
   }

   clear();
}

std::unique_ptr<trogdor::Trogout> StreamOut::clone() {

   return std::make_unique<StreamOut>(stream);
}
