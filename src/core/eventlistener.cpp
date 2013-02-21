#include "include/eventlistener.h"

using namespace std;

namespace core { namespace event {


   void EventListener::execute(const char *event, EventArgumentList args) {

      EventTriggersMap::iterator i = triggers.find(event);

      for (EventTriggerList::iterator j = i->second.begin(); j != i->second.end(); j++) {

         (*j)->execute(args);

         // if we already disabled the action, make sure it stays disabled
         if (allowActionFlag) {
            allowActionFlag = (*j)->allowAction();
         }

         if (!(*j)->continueExecution()) {
            continueExecutionFlag = false;
            break;
         }
      }
   }
}}

