#ifndef EVENTLISTENER_H
#define EVENTLISTENER_H

#include <vector>
#include <memory>
#include <unordered_map>

#include <trogdor/event/eventtrigger.h>

namespace trogdor::event {


   class EventListener {

      private:

         std::unordered_map<std::string, std::vector<std::unique_ptr<EventTrigger>>> triggers;

      public:

         /*
            Constructors for the EventListener class.
         */
         EventListener();
         EventListener(const EventListener &original);
         // TODO: assignment operator that does same thing as copy constructor

         /*
            Adds an EventTrigger to the listener that will be executed whenever
            the listener is invoked.

            Input:
               Event name (std::string)
               Event trigger (EventTrigger)

            Output:
               (none)
         */
         void addTrigger(std::string eventName, std::unique_ptr<EventTrigger> trigger);

         /*
            Executes all EventTriggers for a given event.  

            Input:
               Event (Event)

            Output:
               (none)
         */
         struct EventReturn dispatch(Event e);
   };
}


#endif
