#ifndef RESPAWNEVENTTRIGGER_H
#define RESPAWNEVENTTRIGGER_H


#include <trogdor/event/eventtrigger.h>


namespace trogdor::event {


   class RespawnEventTrigger: public EventTrigger {

      public:

         // The event trigger's name. Used for type comparison.
         static constexpr const char *CLASS_NAME = "RespawnEventTrigger";

         /*
            Registers the event trigger type so that EventTrigger knows how to
            copy and deserialize it later.

            Input:
               (none)

            Output:
               (none)
         */
         static void init();

         // Default, Copy, and deserialization constructors
         RespawnEventTrigger() = default;
         RespawnEventTrigger(const RespawnEventTrigger &t) = default;
         RespawnEventTrigger(const serial::Serializable &data);

         /*
            Returns the class's name.

            Input:
               (none)

            Output:
               Class name (const char *)
         */
         virtual const char *getClassName();

         /*
            Triggers a Being's respawning.

            Input:
               The event that triggered this method call (Event &e)

            Output:
               A pair of flags that determine whether or not execution of event
               listeners an their associated triggers should continue and
               whether or not the action that triggered the event should be
               allowed to continue or be suppressed (EventReturn)
         */
         virtual EventReturn operator()(Event e);

         /*
            Returns a serialized version of the EventTrigger instance.

            Input:
               (none)

            Output:
               Serialized instance of EventTrigger (std::shared_ptr<serial::Serializable>)
         */
         virtual std::shared_ptr<serial::Serializable> serialize();
   };
}


#endif
