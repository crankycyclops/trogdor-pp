#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include <trogdor/event/event.h>

namespace trogdor::event {


   class EventHandler {

      public:

         /*
            Dispatches an event, invoking all the event listeners associated
            with it. Returns true if the action that triggered the event should
            be allowed to continue and false if it should be suppressed.

            Input:
               The event to be dispatched (Event)

            Output:
               True if the action that triggered the event should continue and
               false if it should be suppressed.
         */
         static bool dispatch(Event e);
   };
}


#endif
