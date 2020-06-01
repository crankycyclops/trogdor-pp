#include <trogdor/event/eventlistener.h>

namespace trogdor::event {


   EventListener::EventListener() {}

   /***************************************************************************/

   EventListener::EventListener(const EventListener &original) {

      // TODO: right now, this just creates a fresh copy. I need to actually do
      // a deep copy. I'm not sure how I'm going to get the correct LuaState to
      // any LuaEventTriggers that might be here...
   }

   /***************************************************************************/

   void EventListener::addTrigger(std::string eventName, std::unique_ptr<EventTrigger> trigger) {

      // Not sure why I can't just write triggers[eventName] = {};
      if (triggers.end() == triggers.find(eventName)) {
         triggers[eventName] = std::vector<std::unique_ptr<EventTrigger>>();
      }

      triggers[eventName].push_back(std::move(trigger));
   }

   /***************************************************************************/

   EventReturn EventListener::dispatch(Event e) {

      // If at any point this gets set to false, we should signal by the
      // return value of this method that the action which triggered the
      // event should be suppressed.
      bool allowAction = true;

      if (triggers.end() != triggers.find(e.getName())) {

         for (auto const &trigger: triggers[e.getName()]) {

            EventReturn rv = (*trigger)(e);

            // Once one event trigger has signaled that the action triggering
            // the event should be suppressed, this value should no longer be
            // changed.
            if (rv.allowAction) {
               allowAction = rv.allowAction;
            }

            // No further event triggers should be executed; return imemdiately
            if (!rv.continueExecution) {
               return {allowAction, false};
            }
         }
      }

      return {allowAction, true};
   }
}
