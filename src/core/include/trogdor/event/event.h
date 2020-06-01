#ifndef EVENT_H
#define EVENT_H

#include <list>
#include <trogdor/event/eventarg.h>

namespace trogdor::event {


   // Forward declaration
   class EventListener;

   /**************************************************************************/

   class Event {

      private:

         // I'm using std::list for listeners because order is important and I
         // quite frequently have to push a new listener to the front of the list
         std::string name;
         std::list<EventListener *> listeners;
         std::vector<EventArgument> arguments;

      public:

         /*
            Constructor
         */
         Event(
            std::string n,
            std::list<EventListener *> l,
            std::vector<EventArgument> args
         );

         /*
            Return the event's name.

            Input:
               (none)

            Output:
               Event's name (std::string)
         */
         inline const std::string getName() const {return name;}

         /*
            Return vector of listeners that should be triggered by the event.

            Input:
               (none)

            Output:
               Event listeners (std::list<EventListener *>)
         */
         inline const std::list<EventListener *> getListeners() const {return listeners;}

         /*
            Return the event's arguments.

            Input:
               (none)

            Output:
               Event listeners (std::vector<EventArgument>)
         */
         inline const std::vector<EventArgument> getArguments() const {return arguments;}

         /*
            Prepend an event listener to the beginning of the listeners list.
            The use of this method is discouraged, and it should only be used
            when a wrapper around the event listening system needs to add its
            own listener to the list, as is the case in Game::event().

            Input:
               New event listener (EventListener *)

            Output:
               (none)
         */
         inline void prependListener(EventListener *l) {listeners.push_front(l);}
   };
}


#endif
