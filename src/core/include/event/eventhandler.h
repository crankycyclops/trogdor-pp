#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H


#include <string>
#include <stack>
#include <list>

#include "eventlistener.h"


using namespace std;

namespace trogdor { namespace event {


   class EventHandler {

      public:

         typedef list<EventListener *>        EventListenerList;
         typedef vector<EventListenerList *>  EventListStack;

      private:

         // stack of EventListenerLists, which allows us to call nested events
         EventListStack sessions;

      public:

         /*
            Pushes current event handling session onto the stack and sets up a
            new one.  The new session will be automatically destroyed, and the
            old will be automatically restored, when the next call to event()
            completes.

            Input:
               (none)

            Output:
               (none)
         */
         inline void setup() {sessions.push_back(new EventListenerList());}

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

            EventListenerList *listeners = sessions.back();
            listeners->insert(listeners->end(), l);
         }

         /*
            Triggers an event and executes each EventListener attached to the
            event.  EventListeners are removed once execution is complete, so
            they must be added before each call.  Returns true if the action
            that triggered the event should be allowed to continue and false if
            it should be suppressed.

            Input:
               Event name (C string)
               Arguments (reference to vector of EventArguments)

            Output:
               True if the action that triggered the event should continue and
               false if it should be suppressed.
         */
         bool event(const char *event, EventArgumentList &args);

         /*
            Wrapper around the event() function above with no arguments.  Same
            input and output as mentioned above, minus the EventArgumentList.
         */
         bool event(const char *event);
   };
}}


#endif

