#include <trogdor/event/event.h>

namespace trogdor::event {


   Event::Event(
      std::string n,
      std::list<EventListener *> l,
      std::vector<EventArgument> args
   ): name(n), listeners(l), arguments(args) {}
}
