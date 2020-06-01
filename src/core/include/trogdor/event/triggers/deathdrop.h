#ifndef DEATHDROPEVENTTRIGGER_H
#define DEATHDROPEVENTTRIGGER_H


#include <trogdor/event/eventtrigger.h>


namespace trogdor::event {


   class DeathDropEventTrigger: public EventTrigger {

      public:

         /*
            Drops items from a Being's inventory when the Being dies.

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
