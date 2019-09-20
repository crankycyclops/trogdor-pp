#include "../include/event/eventlistener.h"

using namespace std;

namespace trogdor { namespace event {


   void EventListener::add(string event, EventTrigger *trigger) {

      EventTriggerList *triggerList;

      EventTriggersMap::const_iterator i = triggers.find(event);

      if (i == triggers.end()) {
         triggerList = new EventTriggerList();
         triggers[event] = triggerList;
      }

      else {
         triggerList = i->second;
      }

      triggerList->insert(triggerList->end(), trigger);
   }

   /***************************************************************************/

   void EventListener::execute(string event, EventArgumentList args) {

      EventTriggersMap::const_iterator i = triggers.find(event);

      if (i == triggers.end()) {
         return;
      }

      for (EventTriggerList::const_iterator j = i->second->begin();
      j != i->second->end(); j++) {

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

