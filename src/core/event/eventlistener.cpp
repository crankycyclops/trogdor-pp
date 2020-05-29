#include <trogdor/event/eventlistener.h>

namespace trogdor::event {


   EventListener::EventListener() {

      reset();
   }

   /***************************************************************************/

   EventListener::EventListener(const EventListener &original) {

      // TODO: right now, this just creates a fresh copy. I need to actually do
      // a deep copy. I'm not sure how I'm going to get the correct LuaState to
      // any LuaEventTriggers that might be here...
      reset();
   }

   /***************************************************************************/

   EventListener::~EventListener() {

      // TODO: rewrite these for loops as range-based?
      for (EventTriggersMap::iterator i = triggers.begin();
      i != triggers.end(); i++) {

         for (EventTriggerList::iterator j = i->second->begin();
         j != i->second->end(); j++) {
            delete *j;
         }

         delete i->second;
      }
   }

   /***************************************************************************/

   void EventListener::add(std::string event, EventTrigger *trigger) {

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

   void EventListener::execute(std::string event, EventArgumentList args) {

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
}
