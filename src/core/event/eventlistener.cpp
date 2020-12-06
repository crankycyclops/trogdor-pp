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

         // Need to modify this to be like the deserialization constructor,
         // which takes as input a new LuaState for any instance of
         // LuaEventTrigger. I'll also have to accept as input to this copy
         // constructor that argument (so it'll no longer be a normal copy
         // constructor), and I'll have to update the copy constructor of Entity
         // to take this into account.
         for (const auto &trigger: event.second) {
            triggers[event.first].push_back(
               EventTrigger::instantiate(trigger->getClassName(), trigger.get())
            );
         }
      }
   }

   /***************************************************************************/

   EventListener::EventListener(const serial::Serializable &data, const std::shared_ptr<LuaState> &L) {

      const auto deserializedEvents =
         std::get<std::shared_ptr<serial::Serializable>>(*data.get("triggers"));

      for (const auto &event: deserializedEvents->getAll()) {

         const auto deserializedTriggers =
            std::get<std::vector<std::shared_ptr<serial::Serializable>>>(event.second);

         for (const auto &trigger: deserializedTriggers) {

            std::any arg;
            std::string typeName = std::get<std::string>(*trigger->get("type"));

            if (typeid(LuaEventTrigger) == EventTrigger::getType(typeName.c_str())) {
               arg = std::tuple<serial::Serializable, const std::shared_ptr<LuaState> &>({*trigger, L});
            } else {
               arg = *trigger;
            }

            triggers[event.first].push_back(
               EventTrigger::instantiate(typeName.c_str(), arg)
            );
         }
      }
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
