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

      // Dummy instances of specific event triggers used for dynamic type
      // checking (this doesn't feel like good code, but I know no other way.)
      static AutoAttackEventTrigger autoAttackDummy;
      static DeathDropEventTrigger deathDropDummy;
      static RespawnEventTrigger respawnDummy;

      // I have to check the type of each event trigger and insert a new copy of
      // each.
      for (const auto &event: triggers) {

         triggers[event.first] = std::vector<std::unique_ptr<EventTrigger>>();

         for (const auto &trigger: event.second) {

            if (trigger) {

               // Using this instead of typeid(*trigger) satisfies te compiler
               // gods and fixes the following warning: expression with side
               // effects will be evaluated despite being used as an operand to
               // 'typeid'
               auto &t = *trigger.get();

               if (typeid(autoAttackDummy) == typeid(t)) {
                  triggers[event.first].push_back(std::make_unique<AutoAttackEventTrigger>(
                     *dynamic_cast<AutoAttackEventTrigger *>(trigger.get())
                  ));
               }

               else if (typeid(deathDropDummy) == typeid(t)) {
                  triggers[event.first].push_back(std::make_unique<DeathDropEventTrigger>(
                     *dynamic_cast<DeathDropEventTrigger *>(trigger.get())
                  ));
               }

               else if (typeid(respawnDummy) == typeid(t)) {
                  triggers[event.first].push_back(std::make_unique<RespawnEventTrigger>(
                     *dynamic_cast<RespawnEventTrigger *>(trigger.get())
                  ));
               }

               else {

                  try {
                     triggers[event.first].push_back(std::make_unique<LuaEventTrigger>(
                        *dynamic_cast<LuaEventTrigger *>(trigger.get())
                     ));
                  }

                  catch (const std::bad_cast &e) {
                     throw UndefinedException("Unknown event trigger type encountered in EventListener copy constructor");
                  }
               }
            }
         }
      }
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
