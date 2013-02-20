#include "include/eventlistener.h"

using namespace std;

namespace core { namespace event {


   void EventListener::execute(const char *event, EventArgumentList args) {

      EventTriggersMap::iterator i = triggers.find(event);

      // no triggers found for the specified event
      if (i == triggers.end()) {
         return;
      }

      for (EventTriggerList::iterator j = i->second.begin(); j != i->second.end(); j++) {
         // TODO: call j->execute();
      }
   }
}}

