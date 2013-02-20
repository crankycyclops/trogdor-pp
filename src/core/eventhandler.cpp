#include "include/eventhandler.h"

using namespace std;

namespace core {


   bool EventHandler::event(const char *event, int nArgs, ...) {

      // false if we should not allow the action that triggered the event to
      // continue
      bool allowAction = true;

      for (EventListenerList::iterator i = listeners.begin();
      i != listeners.end(); i++) {
         // TODO
      }

      listeners.clear();
      return allowAction;
   }
}

