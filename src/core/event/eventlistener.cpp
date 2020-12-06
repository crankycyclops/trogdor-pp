#include <trogdor/event/eventlistener.h>

#include <trogdor/event/triggers/autoattack.h>
#include <trogdor/event/triggers/deathdrop.h>
#include <trogdor/event/triggers/respawn.h>
#include <trogdor/event/triggers/luaeventtrigger.h>

#include <trogdor/exception/undefinedexception.h>

namespace trogdor::event {


   EventListener::EventListener() {}

   /***************************************************************************/

   // Necessary because I can't just rely on a default copy constructor for
   // a bunch of unique_ptrs.
   EventListener::EventListener(const EventListener &original) {

      for (const auto &event: triggers) {

         triggers[event.first] = std::vector<std::unique_ptr<EventTrigger>>();

         for (const auto &trigger: event.second) {
            triggers[event.first].push_back(
               EventTrigger::instantiate(trigger->getClassName(), trigger.get())
            );
         }
      }
   }

   /***************************************************************************/

   EventListener::EventListener(const serial::Serializable &data, const std::shared_ptr<LuaState> &L) {

      // TODO: for each listener, if lua, pass in the lua data. Else, pass in
      // serializable directly as the data. The lua state we pass into the
      // second tuple argument for LuaEventTrigger will be L.
   }

   /***************************************************************************/

   std::shared_ptr<serial::Serializable> EventListener::serialize() {

      std::shared_ptr<serial::Serializable> data = std::make_shared<serial::Serializable>();
      std::shared_ptr<serial::Serializable> serializedTriggers = std::make_shared<serial::Serializable>();

      for (auto const &event: triggers) {

         std::vector<std::shared_ptr<serial::Serializable>> eventTriggers;

         for (auto const &trigger: triggers[event.first]) {
            eventTriggers.push_back(trigger->serialize());
         }

         serializedTriggers->set(event.first, eventTriggers);
      }

      data->set("triggers", serializedTriggers);
      return data;
   }

   /***************************************************************************/

   void EventListener::addTrigger(std::string eventName, std::unique_ptr<EventTrigger> trigger) {

      mutex.lock();

      // Not sure why I can't just write triggers[eventName] = {};
      if (triggers.end() == triggers.find(eventName)) {
         triggers[eventName] = std::vector<std::unique_ptr<EventTrigger>>();
      }

      triggers[eventName].push_back(std::move(trigger));
      mutex.unlock();
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
            if (allowAction) {
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
