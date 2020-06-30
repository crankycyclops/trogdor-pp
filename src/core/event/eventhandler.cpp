#include <trogdor/event/eventhandler.h>
#include <trogdor/event/eventlistener.h>

namespace trogdor::event {


   bool EventHandler::dispatch(Event e) {

      // If at any point this gets set to false, we should signal by the
      // return value of this method that the action which triggered the
      // event should be suppressed.
      bool allowAction = true;

      for (auto const listener: e.getListeners()) {

         EventReturn rv = listener->dispatch(e);

         // Once one event trigger has signaled that the action triggering
         // the event should be suppressed, this value should no longer be
         // changed.
         if (allowAction) {
            allowAction = rv.allowAction;
         }

         // No further event triggers should be executed; return imemdiately
         if (!rv.continueExecution) {
            break;
         }
      }

      return allowAction;
   }
}
