#ifndef AUTOATTACKEVENTTRIGGER_H
#define AUTOATTACKEVENTTRIGGER_H


#include <trogdor/event/eventtrigger.h>


namespace trogdor::event {


   class AutoAttackEventTrigger: public EventTrigger {

      public:

         /*
            Scans the room entered by a Being for hostile creatures and
            initiates auto-attack for any creatures that have that feature
            enabled.

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
