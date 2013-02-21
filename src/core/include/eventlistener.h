#ifndef EVENTLISTENER_H
#define EVENTLISTENER_H


#include <string>
#include <boost/unordered_map.hpp>

#include "eventtrigger.h"
#include "luaeventtrigger.h"


using namespace std;
using namespace boost;

namespace core { namespace event {


   class EventListener {

      public:

         typedef list<EventTrigger *> EventTriggerList;
         typedef unordered_map<const char *, EventTriggerList *> EventTriggersMap;

      private:

         // set to false if we wish to suppress the action that originally
         // triggered the event
         int allowActionFlag;

         // set to false if we wish to stop executing any remaining event
         // triggers
         int continueExecutionFlag;

         // Hash table mapping event name => list of EventTriggers
         EventTriggersMap triggers;

      public:

         /*
            Constructor for the EventListener class.
         */
         inline EventListener() {

            allowActionFlag = true;
            continueExecutionFlag = true;
         }

         /*
            Destructor for the EventListener class.
         */
         inline ~EventListener() {

            for (EventTriggersMap::iterator i = triggers.begin();
            i != triggers.end(); i++) {

               for (EventTriggerList::iterator j = i->second->begin();
               j != i->second->end(); j++) {
                  delete *j;
               }

               delete i->second;
            }
         }

         /*
            Returns the state of the allowAction and continueExecution flags.
         */
         inline bool allowAction() const {return allowActionFlag;}
         inline bool continueExecution() const {return continueExecutionFlag;}

         /*
            Adds an EventTrigger to the list of things to execute when the
            specified event is triggered.

            Input:
               event name (C string)
               Pointer to EventTrigger-derived object

            Output:
               (none)
         */
         void add(const char *event, EventTrigger *trigger);

         /*
            Executes all EventTriggers for a given event.  

            Input:
               event name (C string)
               arguments (EventArgumentList)

            Output:
               (none)
         */
         void execute(const char *event, EventArgumentList args);
   };
}}


#endif

