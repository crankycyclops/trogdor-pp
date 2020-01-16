#ifndef DEATHDROPEVENTTRIGGER_H
#define DEATHDROPEVENTTRIGGER_H


#include <trogdor/event/eventtrigger.h>


namespace trogdor::event {


   class DeathDropEventTrigger: public EventTrigger {

      public:

         /*
            Drops items from a Being's inventory when the Being dies.

            Input:
               list of arguments

            Output:
               (none)
         */
         virtual void execute(EventArgumentList args);
   };
}


#endif
