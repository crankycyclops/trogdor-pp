#include <cstdarg>
#include "include/eventhandler.h"

using namespace std;

namespace core { namespace event {


   bool EventHandler::event(const char *event, EventArgumentList &args) {

      // false if we should not allow the action that triggered the event to
      // continue
      bool allowAction = true;

      for (EventListenerList::iterator i = listeners.begin();
      i != listeners.end(); i++) {

         (*i)->execute(event, args);

         // if we turn off allowAction, make sure it stays off
         if (allowAction) {
            allowAction = (*i)->allowAction();
         }

         if (!(*i)->continueExecution()) {
            break;
         }
      }

      listeners.clear();
      return allowAction;
   }

   /***************************************************************************/

   bool EventHandler::event(const char *event) {

      EventArgumentList empty;
      return EventHandler::event(event, empty);
   }
}}

