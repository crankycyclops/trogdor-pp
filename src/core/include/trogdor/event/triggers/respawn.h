#ifndef RESPAWNEVENTTRIGGER_H
#define RESPAWNEVENTTRIGGER_H


#include <trogdor/event/eventtrigger.h>


namespace trogdor::event {


   class RespawnEventTrigger: public EventTrigger {

      public:

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
   };
}


#endif
