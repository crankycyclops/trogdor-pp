#include <cstdarg>
#include "../include/event/eventhandler.h"

namespace trogdor { namespace event {


   bool EventHandler::event(const char *event, EventArgumentList &args) {

      EventListenerList *listeners = sessions.back();

      // false if we should not allow the action that triggered the event to
      // continue
      bool allowAction = true;

      for (EventListenerList::const_iterator i = listeners->begin();
      i != listeners->end(); i++) {

         (*i)->execute(event, args);

         // if we turn off allowAction, make sure it stays off
         if (allowAction) {
            allowAction = (*i)->allowAction();
         }

         (*i)->reset();

         if (!(*i)->continueExecution()) {
            break;
         }
      }

      // restore previous event handling session
      delete listeners;
      sessions.pop_back();

      return allowAction;
   }

   /***************************************************************************/

   bool EventHandler::event(const char *event) {

      EventArgumentList empty;
      return EventHandler::event(event, empty);
   }
}}

