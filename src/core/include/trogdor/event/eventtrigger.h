#ifndef EVENTTRIGGER_H
#define EVENTTRIGGER_H


#include <mutex>
#include <trogdor/event/event.h>
#include <trogdor/serial/serializable.h>


namespace trogdor::event {


   class EventTrigger {

      public:

         /*
            When a class has one or more virtual functions, it should also have
            a virtual destructor.
         */
         virtual ~EventTrigger() = 0;

         /*
            Executes the EventTrigger.

            Input:
               The event that triggered this method call (Event &e)

            Output:
               A pair of flags that determine whether or not execution of event
               listeners an their associated triggers should continue and
               whether or not the action that triggered the event should be
               allowed to continue or be suppressed (EventReturn)
         */
         virtual EventReturn operator()(Event e) = 0;

         /*
            Returns a serialized version of the EventTrigger instance.

            Input:
               (none)

            Output:
               Serialized instance of EventTrigger (std::shared_ptr<serial::Serializable>)
         */
         virtual std::shared_ptr<serial::Serializable> serialize() = 0;
   };
}


#endif
