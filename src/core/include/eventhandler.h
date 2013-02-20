#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H


#include <string>
#include <list>

#include "eventlistener.h"


using namespace std;

namespace core { namespace event {

   class EventHandler {

      public:

         typedef list<EventListener *> EventListenerList;

      private:

         // list of EventListener objects
         EventListenerList listeners;

      public:

         /*
            Adds an EventListener, which will listen for the next executed
            event.  Note that EventListeners are removed everytime a call to
            event() returns.

            Input:
               Pointer to EventListener

            Output:
               (none)
         */
         inline void addListener(EventListener *l) {

            listeners.insert(listeners.begin(), l);
         }

         /*
            Triggers an event and executes each EventListener attached to the
            event.  EventListeners are removed once execution is complete, so
            they must be added before each call.  Returns true if the action
            that triggered the event should be allowed to continue and false if
            it should be suppressed.

            Input:
               Event name (C string)
               Number of arguments (int)
               Arguments (variadic)

            Output:
               True if the action that triggered the event should continue and
               false if it should be suppressed.
         */
         bool event(const char *event, int nArgs, ...);
   };
}}


#endif

