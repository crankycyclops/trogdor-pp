#ifndef EVENTLISTENER_H
#define EVENTLISTENER_H

#include <vector>
#include <memory>
#include <unordered_map>
#include <mutex>

#include <trogdor/event/eventtrigger.h>
#include <trogdor/serial/serializable.h>

namespace trogdor::event {


   class EventListener {

      private:

         // Lock on this for thread-safety
         std::mutex mutex;

         // Vector of event triggers, indexed by event name
         std::unordered_map<std::string, std::vector<std::unique_ptr<EventTrigger>>> triggers;

      public:

         /*
            Constructors for the EventListener class.
         */
         EventListener();
         EventListener(const EventListener &original);
         EventListener(const serial::Serializable &data);
         // TODO: assignment operator that does same thing as copy constructor

         /*
            Returns an easily serializable version of an instance of EventListener.

            Input:
               (none)

            Output:
               A version of an EventListener object in an easily serializable format
         */
         std::shared_ptr<serial::Serializable> serialize();

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
            Returns all triggers currently contained in the event listener.

            Input:
               (none)

            Output:
               Const reference to all triggers
         */
         const auto &getTriggers() const {return triggers;}

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
